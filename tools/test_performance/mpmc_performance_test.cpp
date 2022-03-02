#include <iostream>
#include "stream.h"

#include <thread>
#include "../lock_free_container/mpmc_queue.h"
#include "boost/lockfree/queue.hpp"

#include "timer_guard.h"
#include "rand.h"

using namespace tools;


static pthread_barrier_t barrier_start;

int32_t push_times;
int32_t pop_times;

//#define DEBUG

void test_for_tools_mpmc()
{
    MPMCQueue<size_t>* queue = create_mpmc_queue<size_t>(8192);
#ifndef DEBUG
    pthread_barrier_init(&barrier_start, NULL, 5);
#else
    assert(pthread_barrier_init(&barrier_start, NULL, 5) == 0);
#endif
    {
        std::thread t1([&queue](){
            size_t begin_val = 0;
            auto _push_times = push_times;
            pthread_barrier_wait(&barrier_start);

            while(_push_times > 0)
            {
                if (!queue->push(begin_val))
                {
                    continue;
                }
                begin_val ++;
                _push_times --;
            }
        });

        std::thread t11([&queue](){
            size_t begin_val = 0;
            auto _push_times = push_times;
            pthread_barrier_wait(&barrier_start);

            while(_push_times > 0)
            {
                if (!queue->push(begin_val))
                {
                    continue;
                }
                begin_val ++;
                _push_times --;
            }
        });

        std::thread t2([&queue](){
            size_t get_val = 0;
            auto _pop_times = pop_times;
            pthread_barrier_wait(&barrier_start);
            while(_pop_times > 0)
            {
                if (!queue->pop(get_val))
                {
                    continue;
                }

                _pop_times --;
            }
        });

        std::thread t22([&queue](){
            size_t get_val = 0;
            auto _pop_times = pop_times;
            pthread_barrier_wait(&barrier_start);
            while(_pop_times > 0)
            {
                if (!queue->pop(get_val))
                {
                    continue;
                }

                _pop_times --;
            }
        });

        TimerGuard timer("mpmc_queue");
        pthread_barrier_wait(&barrier_start);

        t1.join();
        t11.join();
        t2.join();
        t22.join();
    }
    free_mpmc_queue(queue);
}


void test_for_boost_queue()
{
    boost::lockfree::queue< size_t,
                            boost::lockfree::fixed_sized<true>,
                            boost::lockfree::capacity<8192>> queue;
    {
#ifndef DEBUG
    pthread_barrier_init(&barrier_start, NULL, 5);
#else
    assert(pthread_barrier_init(&barrier_start, NULL, 5) == 0);
#endif
        std::thread t1([&queue](){
            size_t begin_val = 0;
            auto _push_times = push_times;
            pthread_barrier_wait(&barrier_start);

            while(_push_times > 0)
            {
                if (!queue.bounded_push(begin_val))
                {
                    continue;
                }
                begin_val ++;
                _push_times --;
            }
        });

        std::thread t11([&queue](){
            size_t begin_val = 0;
            auto _push_times = push_times;
            pthread_barrier_wait(&barrier_start);

            while(_push_times > 0)
            {
                if (!queue.bounded_push(begin_val))
                {
                    continue;
                }
                begin_val ++;
                _push_times --;
            }
        });

        std::thread t2([&queue](){
            size_t get_val = 0;
            auto _pop_times = pop_times;
            pthread_barrier_wait(&barrier_start);
            while(_pop_times > 0)
            {
                if (!queue.pop(get_val))
                {
                    continue;
                }

                _pop_times --;
            }
        });

        std::thread t22([&queue](){
            size_t get_val = 0;
            auto _pop_times = pop_times;
            pthread_barrier_wait(&barrier_start);
            while(_pop_times > 0)
            {
                if (!queue.pop(get_val))
                {
                    continue;
                }

                _pop_times --;
            }
        });

        TimerGuard timer("boost_lock_queue");
        pthread_barrier_wait(&barrier_start);

        t1.join();
        t11.join();
        t2.join();
        t22.join();
    }
}

int main()
{
    // 环境参数：
    // Release版，Centos7, gcc 7.5
    // Intel(R) Core(TM) i5-6200U CPU 2核（共4核，分配给虚拟机2个核）
    // 结论：
    // tools::mpmc队列 的性能和 boost库的lock_free::queue队列 性能相比，有明显优势，代价是将额外
    // 使用更多的内存空间来创建定长数据，因为每个数组元素内部也做了cache_line分隔
/*
Tag: mpmc_queue          , diff_nas: 1125520519, As micro: 1125520, As milli: 1125, As sec: 1
Tag: mpmc_queue          , diff_nas: 937759138, As micro: 937759, As milli: 937
Tag: mpmc_queue          , diff_nas: 906751136, As micro: 906751, As milli: 906
Tag: mpmc_queue          , diff_nas: 901995607, As micro: 901995, As milli: 901
Tag: mpmc_queue          , diff_nas: 874879822, As micro: 874879, As milli: 874
Tag: mpmc_queue          , diff_nas: 1284695836, As micro: 1284695, As milli: 1284, As sec: 1
Tag: mpmc_queue          , diff_nas: 785930037, As micro: 785930, As milli: 785
Tag: mpmc_queue          , diff_nas: 826539840, As micro: 826539, As milli: 826
Tag: mpmc_queue          , diff_nas: 936565596, As micro: 936565, As milli: 936
Tag: mpmc_queue          , diff_nas: 862303523, As micro: 862303, As milli: 862
*/
    for (int i = 0 ; i < 10; i++)
    {
        push_times = 10000000;
        pop_times = 10000000;
        test_for_tools_mpmc();
    }

/*
Tag: boost_lock_queue    , diff_nas: 2505291925, As micro: 2505291, As milli: 2505, As sec: 2
Tag: boost_lock_queue    , diff_nas: 2513454361, As micro: 2513454, As milli: 2513, As sec: 2
Tag: boost_lock_queue    , diff_nas: 4850985152, As micro: 4850985, As milli: 4850, As sec: 4
Tag: boost_lock_queue    , diff_nas: 2772822527, As micro: 2772822, As milli: 2772, As sec: 2
Tag: boost_lock_queue    , diff_nas: 2991627255, As micro: 2991627, As milli: 2991, As sec: 2
Tag: boost_lock_queue    , diff_nas: 2423487383, As micro: 2423487, As milli: 2423, As sec: 2
Tag: boost_lock_queue    , diff_nas: 4317837270, As micro: 4317837, As milli: 4317, As sec: 4
Tag: boost_lock_queue    , diff_nas: 2609085618, As micro: 2609085, As milli: 2609, As sec: 2
Tag: boost_lock_queue    , diff_nas: 2479116995, As micro: 2479116, As milli: 2479, As sec: 2
Tag: boost_lock_queue    , diff_nas: 4657293213, As micro: 4657293, As milli: 4657, As sec: 4

*/
    // for (int i = 0 ; i < 10; i++)
    // {
    //     push_times = 10000000;
    //     pop_times = 10000000;
    //     test_for_boost_queue();
    // }
    return 0;
}