#include "stream.h"

#include "../lock_free_container/kfifo.h"

#include "vector.hpp"
#include "spin_lock.h"

#include <atomic>
#include <assert.h>
#include <pthread.h>
#include <thread>
#include <cstring>
#include <stdio.h>
#include <chrono>

using namespace tools;

// 让两个线程分别运行在两个cpu上
int cpu_thread1 = 0;
int cpu_thread2 = 1;

static pthread_barrier_t barrier_start;

void spsc_test()
{
	struct _kfifo  my_fifo3;
	if (ESUCCESS != kfifo_alloc(&my_fifo3, 1024))
		tools::stream << "alloc error" << std::endl;

	tools::stream << "spsc: fifo_size: "<< kfifo_size(&my_fifo3) << std::endl;  

    assert(pthread_barrier_init(&barrier_start, NULL, 3) == 0);

	_kfifo* f_p = &my_fifo3;
	std::thread t1([f_p](){
		char buffer[3];
		for (int i = 0; i < 3; i ++)
			buffer[i] = i + 1;
		pthread_barrier_wait(&barrier_start);
		while(1)
		{
			while (kfifo_avail(f_p) < 1022) {}
				//PAUSE();
			assert(kfifo_in(f_p, buffer, 3) == 3);
		}
	});

	std::thread t2([f_p](){
		char buffer[3];
		memset(buffer, 0, 3);
		pthread_barrier_wait(&barrier_start);
		while(1)
		{
			while(kfifo_len(f_p) < 3) {}
				// PAUSE();
			assert(kfifo_out(f_p, buffer, 3) == 3);
			assert(buffer[0] == 1);
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
    assert(pthread_setaffinity_np(t1_ptr, sizeof(cs), &cs) == 0); // 控制线程在哪个cpu核上运行

	auto t2_ptr = t2.native_handle();
    CPU_ZERO(&cs);
    CPU_SET(cpu_thread2, &cs);
    assert(pthread_setaffinity_np(t2_ptr, sizeof(cs), &cs) == 0);

    pthread_barrier_wait(&barrier_start);

	while(1)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		tools::stream <<"kfifo len : "<< kfifo_len(f_p) << std::endl;
	}

	kfifo_free(&my_fifo3);
}

std::atomic_flag lock_stream = ATOMIC_FLAG_INIT;

void append_number(int x)
{
	while (lock_stream.test_and_set(std::memory_order_acquire)) {}
	stream << "thread #" << x << '\n';
	lock_stream.clear();
}

void atomic_flag_test()
{
	Vector<std::thread> threads;
	for (int i=1; i<=10; ++i)
		threads.push_back(std::thread(append_number,i));
	for (auto& th : threads)
		th.join();
}

void append_number2(int x, SpinLock* lock_ptr)
{
	lock_ptr->lock();
	stream << "thread #" << x << '\n';
	lock_ptr->unlock();
}

void spin_lock_test()
{
	Vector<std::thread> threads;
	SpinLock  lock;
	for (int i=1; i<=10; ++i)
		threads.push_back(std::thread(append_number2,i, &lock));
	for (auto& th : threads)
		th.join();
}

int main()
{
	{
		// 定义
		DECLARE_KFIFO(my_fifo, char, 1024);

		INIT_KFIFO(my_fifo);
	}
	{
		DEFINE_KFIFO(my_fifo2, char, 1024);
	}

	{
		struct _kfifo  my_fifo3;
		if (ESUCCESS != kfifo_alloc(&my_fifo3, 1024))
			tools::stream << "alloc error" << std::endl;
		kfifo_free(&my_fifo3);

		tools::stream << "struct _kfifo : " << sizeof(my_fifo3) << std::endl;
		tools::stream << "struct __kfifo : " << sizeof(struct __kfifo) << std::endl;

		tools::stream << "struct kfifo_rec_ptr_1 : " << sizeof(struct kfifo_rec_ptr_1) << std::endl;
		tools::stream << "struct kfifo_rec_ptr_2 : " << sizeof(struct kfifo_rec_ptr_2) << std::endl;

		// 一个小技巧：在指针内部保存了长度信息
		char (*ptr1)[0];
		char (*ptr2)[1];
		tools::stream << "sizeof *ptr1 " << sizeof(*ptr1) << std::endl;  // 0
		tools::stream << "sizeof *ptr2 " << sizeof(*ptr2) << std::endl;  // 1


		tools::stream <<  (1 << (1 << 3)) - 1 << std::endl;
		tools::stream <<  (1 << (2 << 3)) - 1 << std::endl;
	}

	{
		struct _kfifo  my_fifo4[4];
		if (ESUCCESS != kfifo_alloc(my_fifo4 + 1, 1024))
			tools::stream << "alloc error" << std::endl;
		kfifo_free(my_fifo4 + 1);
	}

	//spsc_test();

	atomic_flag_test();

	stream << "spin lock test" << std::endl;

	spin_lock_test();

    return 0;
}
