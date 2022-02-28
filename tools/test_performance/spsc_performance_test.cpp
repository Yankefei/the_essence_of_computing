#include <iostream>
#include "stream.h"

#include <thread>
#include "../lock_free_container/spsc_queue.h"
#include "boost/lockfree/spsc_queue.hpp"

#include "timer_guard.h"
#include "rand.h"

using namespace tools;

// 让两个线程分别运行在两个cpu上
int cpu_thread1 = 0;
int cpu_thread2 = 1;

static pthread_barrier_t barrier_start;

int32_t push_times;
int32_t pop_times;

//#define DEBUG

void test_for_tools_spsc()
{
    SPSCQueue<char>* queue = create_spsc_queue<char>(8192);
#ifndef DEBUG
    pthread_barrier_init(&barrier_start, NULL, 3);
#else
    assert(pthread_barrier_init(&barrier_start, NULL, 3) == 0);
#endif

    {
        std::thread t1([queue](){
            char buffer[10];
            for (int i = 0; i < 10; i ++)
                buffer[i] = i + 1;
            pthread_barrier_wait(&barrier_start);
            while(push_times -- > 0)
            {
                while (queue->avail() < 10) { }
#ifndef DEBUG
                queue->push(buffer, 10);
#else
                //assert(queue->push(buffer, 10) == 10);
#endif

            }
        });

        std::thread t2([queue](){
            char buffer[10];
            memset(buffer, 0, 10);
            pthread_barrier_wait(&barrier_start);

            while(pop_times -- > 0)
            {
                while(queue->size() < 10 ) { }

#ifndef DEBUG
                queue->get(buffer, 10);
                // for (int i = 0; i < 10; i++)
                //     if (buffer[i] != i + 1)
                //         stream << "error: " << i << std::endl;
#else
                assert(queue->get(buffer, 10) == 10);
#endif
                // for (int i = 0; i < 10; i++)
                //     assert(buffer[i] == i + 1);
                memset(buffer, 0, 10);
            }
        });

        auto t1_ptr = t1.native_handle();
        cpu_set_t cs;
        CPU_ZERO(&cs);
        CPU_SET(cpu_thread1, &cs);
#ifndef DEBUG
        pthread_setaffinity_np(t1_ptr, sizeof(cs), &cs);
#else
        assert(pthread_setaffinity_np(t1_ptr, sizeof(cs), &cs) == 0); // 控制线程在哪个cpu核上运行
#endif

        auto t2_ptr = t2.native_handle();
        CPU_ZERO(&cs);
        CPU_SET(cpu_thread2, &cs);
#ifndef DEBUG
        pthread_setaffinity_np(t2_ptr, sizeof(cs), &cs);
#else
        assert(pthread_setaffinity_np(t2_ptr, sizeof(cs), &cs) == 0);
#endif

        TimerGuard timer("tools_spsc");
        pthread_barrier_wait(&barrier_start);

        t1.join();
        t2.join();
    }
    //stream << "free tools queue" << std::endl;
    free_spsc_queue(queue);
}

void test_for_boost_spsc()
{
    boost::lockfree::spsc_queue<char, boost::lockfree::capacity<8192> > queue;
    {
#ifndef DEBUG
    pthread_barrier_init(&barrier_start, NULL, 3);
#else
    assert(pthread_barrier_init(&barrier_start, NULL, 3) == 0);
#endif
        std::thread t1([&queue](){
            char buffer[10];
            for (int i = 0; i < 10; i ++)
                buffer[i] = i + 1;
            pthread_barrier_wait(&barrier_start);
            while(push_times -- > 0)
            {
                while (queue.write_available() < 10) {}

#ifndef DEBUG
                queue.push(buffer, 10);
#else
                assert(queue.push(buffer, 10) == 10);
#endif
            }
        });

        std::thread t2([&queue](){
            char buffer[10];
            memset(buffer, 0, 10);
            pthread_barrier_wait(&barrier_start);

            while(pop_times -- > 0)
            {
                while(queue.read_available() < 10 ) {}

#ifndef DEBUG
                queue.pop(buffer, 10);
#else
                assert(queue.pop(buffer, 10) == 10);
#endif
                // for (int i = 0; i < 10; i++)
                //     assert(buffer[i] == i + 1);
                memset(buffer, 0, 10);
            }
        });

        auto t1_ptr = t1.native_handle();
        cpu_set_t cs;
        CPU_ZERO(&cs);
        CPU_SET(cpu_thread1, &cs);
#ifndef DEBUG
        pthread_setaffinity_np(t1_ptr, sizeof(cs), &cs);
#else
        assert(pthread_setaffinity_np(t1_ptr, sizeof(cs), &cs) == 0); // 控制线程在哪个cpu核上运行
#endif

        auto t2_ptr = t2.native_handle();
        CPU_ZERO(&cs);
        CPU_SET(cpu_thread2, &cs);
#ifndef DEBUG
        pthread_setaffinity_np(t2_ptr, sizeof(cs), &cs);
#else
        assert(pthread_setaffinity_np(t2_ptr, sizeof(cs), &cs) == 0);
#endif

        TimerGuard timer("boost_spsc");
        pthread_barrier_wait(&barrier_start);

        t1.join();
        t2.join();
    }
    //stream << "free boost queue" << std::endl;
}

int main()
{
    // 环境参数：
    // Release版，Centos7, gcc 7.5
    // Intel(R) Core(TM) i5-6200U CPU 2核（共4核，分配给虚拟机2个核）
    // 结论：
    // tools::spsc队列 的性能和 boost库的spsc队列 性能基本持平。

    /*
    Tag: tools_spsc          , diff_nas: 526283674, As micro: 526283, As milli: 526
    Tag: tools_spsc          , diff_nas: 575332783, As micro: 575332, As milli: 575
    Tag: tools_spsc          , diff_nas: 527294448, As micro: 527294, As milli: 527
    Tag: tools_spsc          , diff_nas: 554304657, As micro: 554304, As milli: 554
    Tag: tools_spsc          , diff_nas: 543862059, As micro: 543862, As milli: 543
    Tag: tools_spsc          , diff_nas: 486976078, As micro: 486976, As milli: 486
    Tag: tools_spsc          , diff_nas: 553677660, As micro: 553677, As milli: 553
    Tag: tools_spsc          , diff_nas: 487395330, As micro: 487395, As milli: 487
    Tag: tools_spsc          , diff_nas: 554027786, As micro: 554027, As milli: 554
    Tag: tools_spsc          , diff_nas: 520513969, As micro: 520513, As milli: 520

    */
    for (int i = 0 ; i < 10; i++)
    {
        push_times = 10000000;
        pop_times = 10000000;
        test_for_tools_spsc();
    }

    /*
    Tag: boost_spsc          , diff_nas: 562887899, As micro: 562887, As milli: 562
    Tag: boost_spsc          , diff_nas: 406730794, As micro: 406730, As milli: 406
    Tag: boost_spsc          , diff_nas: 569088683, As micro: 569088, As milli: 569
    Tag: boost_spsc          , diff_nas: 465957769, As micro: 465957, As milli: 465
    Tag: boost_spsc          , diff_nas: 618240873, As micro: 618240, As milli: 618
    Tag: boost_spsc          , diff_nas: 491087176, As micro: 491087, As milli: 491
    Tag: boost_spsc          , diff_nas: 554654772, As micro: 554654, As milli: 554
    Tag: boost_spsc          , diff_nas: 462063671, As micro: 462063, As milli: 462
    Tag: boost_spsc          , diff_nas: 559953009, As micro: 559953, As milli: 559
    Tag: boost_spsc          , diff_nas: 458713066, As micro: 458713, As milli: 458
    */
    // for (int i = 0 ; i < 10; i++)
    // {
    //     push_times = 10000000;
    //     pop_times = 10000000;
    //     test_for_boost_spsc();
    // }

    return 0;
}