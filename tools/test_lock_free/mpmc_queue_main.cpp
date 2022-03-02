#include <iostream>
#include "stream.h"

#include <thread>
#include "lock_free_container/mpmc_queue.h"

using namespace tools;

//#define DEBUG

// 让两个线程分别运行在两个cpu上
int cpu_thread1 = 0;
int cpu_thread2 = 1;
size_t increase_val = 0;

static pthread_barrier_t barrier_start;


void mpmc_test()
{
	MPMCQueue<size_t>* queue = create_mpmc_queue<size_t>(8192);

	tools::stream << "mpmc: capacity_size: "<< queue->capacity() << std::endl;

#ifndef DEBUG
    pthread_barrier_init(&barrier_start, NULL, 3);
#else
    assert(pthread_barrier_init(&barrier_start, NULL, 3) == 0);
#endif

    bool run = true;

	std::thread t1([queue, &run]()
    {
		pthread_barrier_wait(&barrier_start);
        while(ACCESS_ONCE(run))
        {
		    if (queue->push(increase_val) == false)
            {
                continue;
            }
            increase_val ++;
        }
	});

	std::thread t2([queue, &run](){
		size_t recv_val = 0;
        size_t temp_val = 0;
		pthread_barrier_wait(&barrier_start);
		while(ACCESS_ONCE(run))
		{
			if (queue->pop(temp_val) == false)
            {
                continue;
            }

#ifndef DEBUG
            if (temp_val != recv_val)
                std::runtime_error("pop error");

            recv_val ++;
#else
            assert(temp_val == recv_val ++);
#endif
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

    pthread_barrier_wait(&barrier_start);

    size_t times = 10;
	while(times -- != 0)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		tools::stream <<"queue len : "<< queue->size() << ", increase_val: "<< increase_val << std::endl;
	}

    run = false;
    t1.join();
    t2.join();

    free_mpmc_queue(queue);
}

int main()
{
    stream <<"_Entry size: " << sizeof(_Entry<size_t>) << std::endl;
    stream <<"_Entry::data_ size: " << sizeof(_Entry<size_t>::data_) << std::endl;
    mpmc_test();

    return 0;
}