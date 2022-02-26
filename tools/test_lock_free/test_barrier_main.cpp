// #include "stream.h"

// using namespace tools;

#include <pthread.h>
#include <assert.h>
 
// -------------------
// 让两个线程分别运行在两个cpu上
int cpu_thread1 = 0;
int cpu_thread2 = 1;
 
volatile int x, y, r1, r2;


#if 1
void start()
{
    r1 = 0;
    r2 = 0;
    x = 0;
    y = 0;
}

void end()
{}

void run1()
{
}

void run2()
{

}

#endif
 

#if 0
// 下面这个例子，因为两个线程同时访问一个变量，内存屏障也没有办法保证最终结果的正确性
// 算是内存屏障的一种局限
void end()
{
    assert(x == 2 && y == 2);
}

void run1()
{
    __asm__ __volatile__("mfence" ::: "memory");
    x = x + 1;
    __asm__ __volatile__("mfence" ::: "memory");
    y = y + 1;
    __asm__ __volatile__("mfence" ::: "memory");
}

void run2()
{
    __asm__ __volatile__("mfence" ::: "memory");
    x = x + 1;
    __asm__ __volatile__("mfence" ::: "memory");
    y = y + 1;
    __asm__ __volatile__("mfence" ::: "memory");
}

#endif

#if 0

void start()
{
    x = r1 = r2 = 0;
    y = 0;
}


void end()
{
    // r1. r2不能同时为0 如果断言失败，则说明run1, run2中赋值给r1, r2的指令均提前执行了
    //assert(!(r1 == 0 && r2 == 0));
    assert(r1 != 0 && r2 != 0);
}
 

void run1()
{
    x = 1;
    //__asm__ __volatile__("lfence" ::: "memory");
    //__asm__ __volatile__("sfence" ::: "memory");
    // 更换为  sfence 还是无法解决这个问题
    //__asm__ __volatile__("mfence" ::: "memory"); // 放置cpu乱序执行指令
    r1 = y;
}
 
void run2()
{
    y = 1;
    //__asm__ __volatile__("lfence" ::: "memory");
    //__asm__ __volatile__("sfence" ::: "memory");
    // 更换为  sfence 还是无法解决这个问题
    //__asm__ __volatile__("mfence" ::: "memory");
    r2 = x;
}

#endif
 
// -------------------
static pthread_barrier_t barrier_start;
static pthread_barrier_t barrier_end;
 
static void* thread1(void*)
{
    while (1) {
        pthread_barrier_wait(&barrier_start);
        run1();
        pthread_barrier_wait(&barrier_end);
    }
    
    return NULL;
}
 
static void* thread2(void*)
{
    while (1) {
        pthread_barrier_wait(&barrier_start);
        run2();
        pthread_barrier_wait(&barrier_end);
    }
    
    return NULL;
}

int main()
{
    // 线程barrier控制三个线程在某个位置等待后统一开始
    assert(pthread_barrier_init(&barrier_start, NULL, 3) == 0);
    assert(pthread_barrier_init(&barrier_end, NULL, 3) == 0);
    
    pthread_t t1;
    pthread_t t2;
    assert(pthread_create(&t1, NULL, thread1, NULL) == 0);
    assert(pthread_create(&t2, NULL, thread2, NULL) == 0);
    
    cpu_set_t cs;
    CPU_ZERO(&cs);
    CPU_SET(cpu_thread1, &cs);
    assert(pthread_setaffinity_np(t1, sizeof(cs), &cs) == 0); // 控制线程在哪个cpu核上运行

    CPU_ZERO(&cs);
    CPU_SET(cpu_thread2, &cs);
    assert(pthread_setaffinity_np(t2, sizeof(cs), &cs) == 0);
    
    while (1) {
        start();
        pthread_barrier_wait(&barrier_start);
        pthread_barrier_wait(&barrier_end);
        end();
    }
    
    return 0;
}