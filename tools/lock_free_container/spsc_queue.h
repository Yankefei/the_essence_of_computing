#ifndef _TOOLS_LOCK_FREE_CONTAINER_SPSC_QUEUE_H_
#define _TOOLS_LOCK_FREE_CONTAINER_SPSC_QUEUE_H_

#include <memory>
#include <cassert>
#include <cstring>

#include "general.h"

#include "queue_base.h"

namespace tools
{

template<typename T, typename Alloc = std::allocator<T>>
class SPSCQueue : protected QueueBase<T, Alloc>
{
    typedef QueueBase<T, Alloc> Queue_Base;

    using Queue_Base::buy_array;
    using Queue_Base::free_array;

public:
    SPSCQueue(uint32_t length)
    {
        if (!IS_POWER_OF_2(length))
            length = tools::round_to_power_of_2(length);
        
        length = length >= 2 ? length : 2;
        buf_ = buy_array(length);
        mask_ = length - 1;
    }

    ~SPSCQueue()
    {
        destory();
    }

    SPSCQueue(const SPSCQueue&) = delete;
    SPSCQueue& operator=(const SPSCQueue&) = delete;
    SPSCQueue(SPSCQueue&&) = delete;
    SPSCQueue& operator=(SPSCQueue&&) = delete;

    void destory()
    {
        if (buf_ != nullptr)
        {
            free_array(buf_);
            buf_ = nullptr;
        }
    }

public:
    // push 中利用in_, out对剩余容量的判断早于数据存储，
    //        对数据的存储晚于 in_指针的递增
    // get 中利用in_, out对剩余元素的判断早于数据的加载，
    //       对数据的加载晚于 out_指针的递增
    // in_ >= out_

    /**
     * 用数轴表示先后顺序即：
     *                               load              store
     * ---------------------out---------------in------------------------
     * 
     *  简单理解，in_，out_标识将store和load的过程严格划分成了两个部分
     * 
    */

    // 插入单个元素
    bool push(const T& val)
    {
        bool res = !is_full();
        if (res)
        {
            buf_[in_ & mask_] = val;
            WMB();
            in_ ++;
        }
        return res;
    }

    // 获取单个元素
    bool get(T* val)
    {
        bool res = !empty();
        if (res)
        {
            *val = buf_[out_ & mask_];
            WMB();
            out_ ++;
        }
        return res;
    }

    uint32_t push(const T* buf, uint32_t len)
    {
        uint32_t l;
        l = unused_ele();

        if (len > l)
            len = l;

        copy_in(buf, len, in_);

        in_ += len;
        return len;
    }

    uint32_t get(T* buf, uint32_t len)
    {
        uint32_t l;
        l = in_ - out_;
        if (len > l)
            len = l;

        copy_out(buf, len, out_);

        out_ += len;
        return len;
    }

    bool empty()
    {
        return ACCESS_ONCE(in_) == ACCESS_ONCE(out_);
    }

    bool is_full()
    {
        return size() > mask_;
    }

    uint32_t size()
    {
        return ACCESS_ONCE(in_) - ACCESS_ONCE(out_);
    }

    uint32_t avail()
    {
        return capacity() - size();
    }

    uint32_t capacity()
    {
        return mask_ + 1;
    }

    // Note: 非线程安全接口
    void reset()
    {
        out_ = in_ = 0;
    }

private:
    void copy_in(const T *src, uint32_t len, uint32_t off)
    {
        uint32_t l;
        uint32_t size = mask_ + 1;

        off &= mask_; // 获取待插入的数组下标
        auto ele_size = sizeof(T);
        if (ele_size != 1)
        {    
            off *= ele_size;
            len *= ele_size;
            size *= ele_size;
        }

        l = MIN(len, size - off);

        memcpy((uint8_t*)buf_ + off, src, l);
        memcpy(buf_, (uint8_t*)src + l, len - l);

	    WMB(); // 写内存屏障
    }

    void copy_out(T *dst, unsigned int len, unsigned int off)
    {
        uint32_t l;
        uint32_t size = mask_ + 1;

        off &= mask_; // 获取待插入的数组下标
        auto ele_size = sizeof(T);
        if (ele_size != 1)
        {    
            off *= ele_size;
            len *= ele_size;
            size *= ele_size;
        }

        l = MIN(len, size - off);

        memcpy(dst, (uint8_t*)buf_ + off, l);
        memcpy((uint8_t*)dst + l, buf_, len - l);

        WMB(); // 写内存屏障
    }

    inline uint32_t unused_ele()
    {
        return (mask_ + 1) - (in_ - out_);
    }

private:
    // 避免伪共享
    uint32_t in_{0};
    EMPTY_CACHE_LINE;

    uint32_t out_{0};
    EMPTY_CACHE_LINE;

    T*         buf_{nullptr};
    uint32_t   mask_{0};
};


template<typename T>
static SPSCQueue<T>* create_spsc_queue(uint32_t length)
{
    SPSCQueue<T>* queue = new SPSCQueue<T>(length);
    return queue;
}

template<typename T>
static void free_spsc_queue(SPSCQueue<T>* queue)
{
    queue->destory();
}

}

#endif