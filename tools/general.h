#ifndef _TOOLS_GENERAL_H_
#define _TOOLS_GENERAL_H_

#include <stdint.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace tools
{


// 定义常用工具宏函数 (linux环境, x86-64指令集)

// 部分引用自 PDPK库的一些定义

#define CACHE_LINE_SIZE      64

#define ALIGN(align_size)   __attribute__((aligned(align_size)))

#define CACHE_LINE_ALIGN    ALIGN(CACHE_LINE_SIZE)

// 用这种方式，可以让 __LINE__在预编译时先被替换
#define CONCATENATE(arg1, arg2)    CONCATENATE1(arg1, arg2)
#define CONCATENATE1(arg1, arg2)   arg1##arg2

// 使用行标__LINE__命名pad的变量名
#define EMPTY_CACHE_LINE    CONCATENATE(char pad, __LINE__) \
                                __attribute__((aligned(CACHE_LINE_SIZE)))

// 生成有利于分支预测的代码, 告知编译器x为true概率较高
#define LIKELY(x)  (__builtin_expect(!!(x), 1))

// 生成有利于分支预测的代码, 告知编译器x为false概率较高
#define UNLIKELY(x)  (__builtin_expect(!!(x), 0))

// 强制编译器生成一条从内存而不是寄存器读取变量x值的CPU指令
#define ACCESS_ONCE(x) (*(volatile decltype(x) *)&(x))

// 编译器屏障, 防止编译器生成乱序的代码
#define BARRIER()    asm volatile ("" : : : "memory")


// Note: 下面的GNU C扩展ASM语句均包含上面的编译器屏障功能
// CPU内存屏障, 防止CPU乱序执行指令, 用于SMP体系中
#define MB()         asm volatile ("mfence" ::: "memory")

// CPU内存屏障, 防止CPU乱序执行内存读指令, 用于SMP体系中
#define RMB()        asm volatile ("lfence" ::: "memory")

// CPU内存屏障, 防止CPU乱序执行内存写入指令, 用于SMP体系中
#define WMB()        asm volatile ("sfence" ::: "memory")

// 产生一条CPU暂停指令，以达到纳秒级delay的效果，用于无锁编程的busy retry环节
#define PAUSE()      asm volatile ("pause" ::: "memory")


// 获取内存页大小
#define PAGE_SIZE    (uint64_t)(getpagesize())
// 设置掩码
#define PAGE_MASK    (~(PAGE_SIZE - 1))

// 这两个地址对齐的宏主要用于内存池的分配策略上
// 将一个内存地址向上对齐到一个内存页的边界, 返回对齐后的地址
#define PAGE_ALIGN(x)  ((decltype(x)(uint64_t)((char*)(x) + PAGE_SIZE - 1) & PAGE_MASK))

// 判断内存地址是否对齐到内存页面大小
#define IS_PAGE_ALIGN(x) (((uint64_t)(x) & (PAGE_SIZE - 1)) == 0ul)

// 获取结构体中某一字段距结构首地址的偏移量
// type  结构体类型,   member 结构中字段的名称
#define OFFSET_OF(type, member) ((size_t)&(((type*)0)->member))

// 获取ptr所指向字段的外部容器结构的首地址
// ptr 指针地址， type 外部容器类型， member ptr在容器定义中的名称
#define CONTAINER_OF(ptr, type, member) ((type*)((char*)(ptr) - OFFSET_OF(type, member)))


// (void)(&_a == &_b) 为了让编译器产生类型不一致的警告
// 保证比较过程中a, b的值均只会被load一次，避免并发问题
#define MIN(a, b) \
	({ \
		decltype (a) _a = (a); \
		decltype (b) _b = (b); \
        (void) (&_a == &_b);   \
		_a < _b ? _a : _b;     \
	})

#define MAX(a, b) \
	({ \
		decltype (a) _a = (a); \
		decltype (b) _b = (b); \
        (void) (&_a == &_b);   \
		_a > _b ? _a : _b;     \
	})


#define IS_POWER_OF_2(x) ((((x)-1) & (x)) == 0)


// 将一个无符号32位整数向上取整到2的N次幂, 忽略0值
static inline uint32_t round_to_power_of_2(uint32_t v)
{
    uint32_t _v = (uint32_t)(v);
    --_v;
    _v |= _v >> 1;
    _v |= _v >> 2;
    _v |= _v >> 4;
    _v |= _v >> 8;
    _v |= _v >> 16;
    ++_v;
    return _v;
}


// 线程ID
#define GET_THREAD_ID    syscall(SYS_gettid)

}

#endif