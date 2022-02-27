#include "stream.h"

#include "vector.hpp"
#include "spin_lock.h"

#include <atomic>
#include <assert.h>
#include <thread>
#include <cstring>
#include <stdio.h>
#include <chrono>

using namespace tools;

Vector<int> queue_data;
std::atomic<int> count;

void populate_queue()
{
  unsigned const number_of_items=100;
  queue_data.clear();
  for(unsigned i=0;i<number_of_items;++i)
  {
    queue_data.push_back(i);
  }

  count.store(number_of_items,std::memory_order_release);  // 1 初始化存储
}

void process(int data, int id)
{
    stream <<"id: "<< id << " " << data << std::endl;
}

void consume_queue_items()
{
  int id = GET_THREAD_ID;
  while(true)
  {
    int item_index;
    /*
        你想要同时进行获取和释放的语义，所以memory_order_acq_rel是一个合适的选择，但你也可以使用其他序列。
        使用memory_order_acquire语义的fetch_sub是不会和任何东西同步的，即使它存储了一个值，这是因为其没有释放操作。
        同样的，使用memory_order_release语义的fetch_or也不会和任何存储操作进行同步，因为对于fetch_or的读取，
        并不是一个获取操作。
        -- by 《C++ Concurrency in Action》
    */
    // 幸运的是，第一个fetch_sub()对释放顺序做了一些事情，所以store()能同步与第二个fetch_sub()操作。
    // 这里，两个消费者线程间不需要同步关系。
    if((item_index=count.fetch_sub(1,std::memory_order_acquire))<=0)  // 2 一个“读-改-写”操作
    {

      //wait_for_more_items();  // 3 等待更多元素
      continue;
    }
    process(queue_data[item_index-1], id);  // 4 安全读取queue_data
  }
}



struct AppleD
{
  int color = 0;
  int weight = 3;
  int size = 5;
};


void atomic_test()
{
	stream << Boolalpha;
	{
		// compile error if unlink -latomic
		AppleD b;
		std::atomic<AppleD> a(b);
		stream << a.load().size << std::endl;
		bool is_lock_free = a.is_lock_free();
		stream <<"ApplD lock_free: "<< is_lock_free << std::endl;  // false;

		std::atomic<bool> test_a{true};
		is_lock_free = test_a.is_lock_free();
		stream <<"bool lock_free: " << is_lock_free << std::endl;   // true

		std::atomic<int64_t> test_b{true};
		is_lock_free = test_b.is_lock_free();
		stream <<"int64_t lock_free: " << is_lock_free << std::endl;   // true
	}
	{
		std::atomic<int> test_a(10); 
		int expected_a = 10;
		if (atomic_compare_exchange_strong(&test_a, &expected_a, 20))
		{
			stream << "change :" <<test_a.load()<<"  " <<expected_a<< std::endl;  // 20 . 10
		}
	}

	{
		std::atomic<int> test_a(10); 
		int expected_a = 30;
		if (!atomic_compare_exchange_strong(&test_a, &expected_a, 20))
		{
			stream << "un change :" <<test_a.load()<<"  " <<expected_a<< std::endl; // 10 10
		}
	}


	{
		// atomic 提供的屏障函数
		std::atomic_signal_fence(std::memory_order_acquire);
		std::atomic_thread_fence(std::memory_order_acquire);
	}
}


void populate_consume_test()
{
  std::thread a(populate_queue);
  std::thread b(consume_queue_items);
  std::thread c(consume_queue_items);
  a.join();
  b.join();
  c.join();
}



std::atomic<int> accumulate_val{0};
int accumulate_val2 = 0;

void atomic_accumulate_test()
{
    std::thread a([]()
    {
        for (int i = 0; i < 1000000; i++)
            accumulate_val.fetch_add(1, std::memory_order_relaxed);
    });
    std::thread b([]()
    {
        for (int i = 0; i < 1000000; i++)
            accumulate_val.fetch_add(1, std::memory_order_relaxed);
    });

    a.join();
    b.join();

    // 说明原子变量即使内存模型为最宽松的那种，但是也仅仅指的是其执行顺序，但具体操作还是原子类型
    assert(accumulate_val.load() == 2000000);
}

void accumulate_test()
{
    std::thread a([]()
    {
        for (int i = 0; i < 1000000; i++)
            accumulate_val2 ++;
    });
    std::thread b([]()
    {
        for (int i = 0; i < 1000000; i++)
            accumulate_val2 ++;
    });

    a.join();
    b.join();

    // failed
    // assert(accumulate_val2 == 2000000);
    assert(accumulate_val2 != 2000000);
}

int main()
{
    // populate_consume_test();
    
    // atomic_test();

    atomic_accumulate_test();
    accumulate_test();

    return 0;
}



#if 0

std::atomic<int> x(0),y(0),z(0);  // 1
std::atomic<bool> go(false);  // 2

unsigned const loop_count=10;

struct read_values
{
  int x,y,z;
};

read_values values1[loop_count];
read_values values2[loop_count];
read_values values3[loop_count];
read_values values4[loop_count];
read_values values5[loop_count];

// var_to_inc  x, y, z
void increment(std::atomic<int>* var_to_inc,read_values* values)
{
  while(!go)
    std::this_thread::yield();  // 3 自旋，等待信号
  for(unsigned i=0;i<loop_count;++i)
  {
    values[i].x=x.load(std::memory_order_relaxed);
    values[i].y=y.load(std::memory_order_relaxed);
    values[i].z=z.load(std::memory_order_relaxed);
    var_to_inc->store(i+1,std::memory_order_relaxed);  // 4
    std::this_thread::yield();
  }
}

void read_vals(read_values* values)
{
  while(!go)
    std::this_thread::yield(); // 5 自旋，等待信号
  for(unsigned i=0;i<loop_count;++i)
  {
    values[i].x=x.load(std::memory_order_relaxed);
    values[i].y=y.load(std::memory_order_relaxed);
    values[i].z=z.load(std::memory_order_relaxed);
    std::this_thread::yield();
  }
}

void print(read_values* v)
{
  for(unsigned i=0;i<loop_count;++i)
  {
    if(i)
      stream <<",";
    stream <<"("<<v[i].x<<","<<v[i].y<<","<<v[i].z<<")";
  }
  stream <<std::endl;
}

int main()
{
  std::thread t1(increment,&x,values1);
  std::thread t2(increment,&y,values2);
  std::thread t3(increment,&z,values3);
  std::thread t4(read_vals,values4);
  std::thread t5(read_vals,values5);

  go=true;  // 6 开始执行主循环的信号

  t5.join();
  t4.join();
  t3.join();
  t2.join();
  t1.join();

/*
(0,0,0),(1,2,1),(2,3,1),(3,10,9),(4,10,9),(5,10,9),(6,10,9),(7,10,9),(8,10,9),(9,10,9)
(1,0,0),(1,1,1),(2,2,1),(3,3,1),(3,4,2),(3,5,3),(3,6,4),(3,7,5),(3,8,6),(3,9,6)
(1,1,0),(3,4,1),(3,5,2),(3,6,3),(3,7,4),(3,8,5),(3,10,6),(3,10,7),(3,10,8),(10,10,9)
(3,9,6),(3,10,7),(3,10,8),(3,10,9),(3,10,9),(3,10,9),(4,10,9),(4,10,9),(5,10,9),(5,10,9)
(3,10,9),(3,10,9),(3,10,9),(4,10,9),(5,10,9),(5,10,9),(6,10,9),(7,10,9),(8,10,9),(9,10,9)

*/

  print(values1);  // 7 打印最终结果
  print(values2);
  print(values3);
  print(values4);
  print(values5);

}


#endif