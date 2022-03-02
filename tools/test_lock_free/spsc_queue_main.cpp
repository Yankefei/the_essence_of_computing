#include <iostream>
#include "stream.h"

#include <thread>
#include "lock_free_container/spsc_queue.h"

using namespace tools;

//#define DEBUG

// 让两个线程分别运行在两个cpu上
int cpu_thread1 = 0;
int cpu_thread2 = 1;

static pthread_barrier_t barrier_start;

void spsc_test()
{
	SPSCQueue<char>* queue = create_spsc_queue<char>(1024);

	tools::stream << "spsc: fifo_size: "<< queue->capacity() << std::endl;  

    pthread_barrier_init(&barrier_start, NULL, 3);
    bool run = true;

	std::thread t1([queue, &run](){
		char buffer[3];
		for (int i = 0; i < 3; i ++)
			buffer[i] = i + 1;
		pthread_barrier_wait(&barrier_start);
		while(ACCESS_ONCE(run))
		{
			while (queue->avail() < 3 && ACCESS_ONCE(run)) {}
				//PAUSE();

#ifndef DEBUG
			if (ACCESS_ONCE(run) && queue->push(buffer, 3) != 3)
			{
				std::runtime_error("push error");
			}
#else
			assert(ACCESS_ONCE(run) && queue->push(buffer, 3) == 3);			
#endif
		}
	});

	std::thread t2([queue, &run](){
		char buffer[3];
		memset(buffer, 0, 3);
		pthread_barrier_wait(&barrier_start);
		while(ACCESS_ONCE(run))
		{
			while(queue->size() < 3 && ACCESS_ONCE(run)) {}
				// PAUSE();
            if (LIKELY(ACCESS_ONCE(run)))
            {
#ifndef DEBUG
				if (!(queue->get(buffer, 3) == 3 && buffer[0] == 1))
				{
					std::runtime_error("push error");
				}
#else
                assert(queue->get(buffer, 3) == 3);
                assert(buffer[0] == 1);
#endif
            }
			// assert(buffer[1] == 2);
			// assert(buffer[2] == 3);
			// for (int i = 0; i < 10; i++)
			// 	assert(buffer[i] == i + 1);
			memset(buffer, 0, 3);
		}
	});

	auto t1_ptr = t1.native_handle();
    cpu_set_t cs;
    CPU_ZERO(&cs);
    CPU_SET(cpu_thread1, &cs);
    pthread_setaffinity_np(t1_ptr, sizeof(cs), &cs); // 控制线程在哪个cpu核上运行

	auto t2_ptr = t2.native_handle();
    CPU_ZERO(&cs);
    CPU_SET(cpu_thread2, &cs);
    pthread_setaffinity_np(t2_ptr, sizeof(cs), &cs);

    pthread_barrier_wait(&barrier_start);

    size_t times = 30;
	while(times -- != 0)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		tools::stream <<"kfifo len : "<< queue->size() << std::endl;
	}


    run = false;
    t1.join();
    t2.join();
	free_spsc_queue(queue);
}

int main()
{
    spsc_test();
    return 0;
}