#ifndef _TOOLS_SPIN_LOCK_H_
#define _TOOLS_SPIN_LOCK_H_

#include <memory>
#include <cassert>

#include "general.h"

namespace tools
{

/**
 * 简易自旋锁, 参考《深入理解c++11, c++11新特性解析与应用》
 */
class SpinLock
{
public:
    SpinLock() = default;
    ~SpinLock() = default;

    SpinLock(const SpinLock&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;
    SpinLock(SpinLock&&) = delete;
    SpinLock& operator=(SpinLock&&) = delete;

    // lock 和 unlock的调用必须保证代码级别的先后顺序
    void lock()
    {
        while(lock_.test_and_set(std::memory_order_acquire)) //尝试获取自旋锁，线程内后续读操作后置
        {}
    }

    void unlock()
    {
        lock_.clear(std::memory_order_release); // 清理标志，线程内之后的写操作前置
    }

private:
    std::atomic_flag lock_{ATOMIC_FLAG_INIT};
};

}


#endif