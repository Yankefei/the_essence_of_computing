#ifndef _TOOLS_LOCK_FREE_CONTAINER_SPSC_QUEUE_H_
#define _TOOLS_LOCK_FREE_CONTAINER_SPSC_QUEUE_H_

#include <memory>
#include <cassert>
#include <cstring>

#include "general.h"

namespace tools
{

template<typename T, typename Alloc = std::allocator<T>>
class SPSCQueue
{
    // 一次申请的内存数为1字节
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<uint8_t> _Byte_alloc_type;   
    typedef typename std::allocator_traits<_Byte_alloc_type>
        byte_rebind_traits;
    typedef typename byte_rebind_traits::pointer  byte_pointer;

    // 一次申请的内存数为 T大小
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<T> _T_alloc_type;
    typedef typename std::allocator_traits<_T_alloc_type>
        t_rebind_traits;
    typedef typename t_rebind_traits::pointer t_pointer;

    struct ByteBase_Impl : public _Byte_alloc_type {};  // 仅仅用来指导申请内存
    static ByteBase_Impl _m_byte_impl_;                 // 无状态，设置为静态变量

    struct TBase_Impl : public _T_alloc_type {};  // 仅仅用来指导申请内存
    static TBase_Impl _m_t_impl_;                 // 无状态，设置为静态变量

    // 入参为需要申请的数组长度
    T* buy_array(uint32_t size)
    {
        if (size == 0) return nullptr;
        size_t total_size = size * sizeof(T) + 4;
        byte_pointer b_p = byte_rebind_traits::allocate(_m_byte_impl_, total_size);
        memset(b_p, 0, total_size);
        uint32_t* s_p = reinterpret_cast<uint32_t*>(b_p);
        *s_p = size; // 数组头部放置数组的长度信息

        // 构造T类型数据
        b_p += 4;
        for (int i = 0; i < size; i++)
        {
            t_rebind_traits::construct(_m_t_impl_, b_p + i * sizeof(T));
        }

        return reinterpret_cast<T*>(s_p + 1);
    }

    void free_array(T* array)
    {
        assert(array != nullptr);
        uint32_t* s_p = reinterpret_cast<uint32_t*>(array) - 1;
        uint32_t array_len = *s_p;
        for (int i = 0; i < array_len; i++)
        {
            t_rebind_traits::destroy(_m_t_impl_, array + i);
        }
        size_t total_size = array_len * sizeof(T) + 4;
        byte_rebind_traits::deallocate(_m_byte_impl_,
            reinterpret_cast<byte_pointer>(s_p), total_size);
    }

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
        return in_ == out_;
    }

    bool is_full()
    {
        return size() > mask_;
    }

    uint32_t size()
    {
        return in_ - out_;
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

template <typename T, typename Alloc>
typename SPSCQueue<T, Alloc>::ByteBase_Impl SPSCQueue<T, Alloc>::_m_byte_impl_;

template <typename T, typename Alloc>
typename SPSCQueue<T, Alloc>::TBase_Impl SPSCQueue<T, Alloc>::_m_t_impl_;


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