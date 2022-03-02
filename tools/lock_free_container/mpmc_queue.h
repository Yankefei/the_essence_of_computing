#ifndef _TOOLS_LOCK_FREE_CONTAINER_MPMC_QUEUE_H_
#define _TOOLS_LOCK_FREE_CONTAINER_MPMC_QUEUE_H_

#include <memory>
#include <cassert>
#include <cstring>
#include <cmath>
#include <atomic>

#include "general.h"

#include "queue_base.h"

namespace tools
{

template<typename T>
struct _Entry
{
    // 避免伪共享
    // 标定目前的数据已经属于环形队列的多少轮, 奇数表示被使用，偶数表示数据为空（第一圈从0开始）：
    // 0,1 表示第一轮，2,3 表示第二轮. 以此类推
    CACHE_LINE_ALIGN std::atomic<size_t> turn_{0};    
    typename std::aligned_storage<sizeof(T), alignof(T)>::type data_; // 定义T类型数组，但不调用T的构造函数

    _Entry() = default;    
    ~_Entry()
    {
        if (turn_ & 1)
            destory();
    }

    // 显式构造T数据
    template<typename... Args>
    void construct_T(Args&& ...args) noexcept
    {
        new (&data_) T(std::forward<Args>(args)...);
    }

    T&& move() noexcept
    {
        return reinterpret_cast<T &&>(data_);
    }

    void destory()
    {
        reinterpret_cast<T *>(&data_)->~T();
    }
};

// mpmc_queue
template<typename T, typename Entry = _Entry<T>, typename Alloc = std::allocator<T>>
class MPMCQueue : protected QueueBase<Entry, Alloc>
{
    typedef QueueBase<Entry, Alloc> Queue_Base;

    using Queue_Base::buy_array_no_construct;
    using Queue_Base::free_array_no_destruct;

public:
    MPMCQueue(uint32_t length)
    {
        if (!IS_POWER_OF_2(length))
            length = tools::round_to_power_of_2(length);
        
        length = length >= 2 ? length : 2;
        buf_ = buy_array_no_construct(length);
        for (size_t i = 0; i < length; ++i)
        {
            new (&buf_[i]) Entry();
        }
        mask_ = length - 1;
        power_of_length_ = log2(length);
    }

    ~MPMCQueue()
    {
        destory();
    }

    MPMCQueue(const MPMCQueue&) = delete;
    MPMCQueue& operator=(const MPMCQueue&) = delete;
    MPMCQueue(MPMCQueue&&) = delete;
    MPMCQueue& operator=(MPMCQueue&&) = delete;

    bool push(const T& v)
    {
        return try_emplace(v);
    }

    bool emplace(T&& v) noexcept
    {
        return try_emplace(std::forward<T>(v));
    }

    // 插入成功返回true, 队列已满返回false
    template<typename... Args>
    bool try_emplace(Args &&...args) noexcept
    {
        auto in = in_.load(std::memory_order_acquire);
        for (;;)
        {
            auto &entry = buf_[index(in)];
            if ((turn(in) << 1) == entry.turn_.load(std::memory_order_acquire))
            {
                if (in_.compare_exchange_strong(in, in + 1)) // 直接累加
                {
                    entry.construct_T(std::forward<Args>(args)...);
                    entry.turn_.store((turn(in) << 1) + 1, std::memory_order_release);
                    return true;
                }
            }
            else
            {
                auto prev_in = in;
                in = in_.load(std::memory_order_acquire); // 重新加载
                if (in == prev_in)  // in 不再变化，说明队列已满
                    return false;
            }
        }
    }

    // 出队成功返回true, 队列为空返回false
    bool pop(T& v)
    {
        auto out = out_.load(std::memory_order_acquire);
        for (;;)
        {
            auto &entry = buf_[index(out)];
            if ((turn(out) << 1) + 1 == entry.turn_.load(std::memory_order_acquire))
            {
                if (out_.compare_exchange_strong(out, out + 1))
                {
                    v = entry.move();
                    entry.destory();
                    entry.turn_.store((turn(out) << 1) + 2, std::memory_order_release);
                    return true;
                }
            }
            else
            {
                auto prev_out = out;
                out = out_.load(std::memory_order_acquire);
                if (out == prev_out)  // 相等则说明队列为空
                    return false;
            }
        }
    }

    // 多线程下可能为负数
    ptrdiff_t size() const
    {
        return static_cast<ptrdiff_t>(in_.load(std::memory_order_relaxed) -
                                      out_.load(std::memory_order_relaxed));
    }

    // 多线程下可能为负数
    bool empty() const
    {
        return size() <= 0;
    }

    bool capacity() const
    {
        return mask_ + 1;
    }

private:
    // 获取当前值i的索引下标
    size_t index(size_t i)
    {
        return i & mask_;
    }

    // 获取当前循环了环形队列的轮数, 从0轮开始递增
    size_t turn(size_t i)
    {
        return i >> power_of_length_;
    }

    void destory()
    {
        if (buf_ != nullptr)
        {
            for (size_t i = 0; i < mask_ + 1; ++i)
                buf_[i].~Entry();  // 显式调用析构函数
            free_array_no_destruct(buf_);
            buf_ = nullptr;
        }
    }

private:
    // 避免伪共享
    std::atomic<size_t>     in_;
    EMPTY_CACHE_LINE;

    std::atomic<size_t>     out_;
    EMPTY_CACHE_LINE;

    Entry*                  buf_;    // Entry数组
    size_t                  mask_;
    size_t                  power_of_length_;  // 长度的幂值
};


template<typename T>
static MPMCQueue<T>* create_mpmc_queue(uint32_t length)
{
    MPMCQueue<T>* queue = new MPMCQueue<T>(length);
    return queue;
}

template<typename T>
static void free_mpmc_queue(MPMCQueue<T>* queue)
{
    queue->~MPMCQueue<T>();
}

}

#endif