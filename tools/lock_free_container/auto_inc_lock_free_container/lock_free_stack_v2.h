#ifndef _TOOLS_LOCK_FREE_CONTAINER_STACK_V2_H_
#define _TOOLS_LOCK_FREE_CONTAINER_STACK_V2_H_

#include <memory>
#include <cassert>

#include <functional>
#include <thread>
#include <atomic>

namespace tools
{

template<typename T>
class lock_free_stack_
{
private:
	struct node
	{
		std::shared_ptr<T> data;
		std::shared_ptr<node> next;
		node(const T& data_) : data(std::make_shared<T>(data_)) {}
	};

private:
	std::shared_ptr<node> head;

public:
	void push(const T& data)
	{
		std::shared_ptr<node> const new_node = std::make_shared<node>(data);
		new_node->next = std::atomic_load(&head);
		while (!std::atomic_compare_exchange_weak(&head, &new_node->next, new_node));
	}

	std::shared_ptr<T> pop()
	{
		std::shared_ptr<node> old_head = std::atomic_load(&head);
		while (old_head &&
            !std::atomic_compare_exchange_weak(&head, &old_head,
                std::atomic_load(&old_head->next))); // 1 这里使用了atomic_load
		if (old_head)
		{
			std::atomic_store(&old_head->next, std::shared_ptr<node>(nullptr)); // 2 注意这行操作
			return old_head->data;
		}

		return nullptr;
	}

	~lock_free_stack_()
	{
		while (pop());
	}
};


template<typename T>
class lock_free_stack
{
private:
	struct node;
	struct counted_node_ptr
	{
		// 注意，外部计数器用的是普通变量，因为外部计数器(或者说这个counted_node_ptr结构体对象)
		// 一般都是拷贝副本，并不会对单一对象执行并发访问
		int external_count = 0; 
		node* ptr = nullptr;
	};

    /*
        读取这个节点的指针的时候，外部计数器加1，读取完成后，内部计数器减1。当某个节点从stack中取出来后
        (此时这个节点在其他线程中可能被引用了，也可能没被引用)，此节点的内部计数器就要加上此节点的外部
        计数器的值并减去1(internal_counter = external_counter - 1)。一旦内部计数器的值为0，就表示没有
        其他线程引用这个节点了，只有当前线程引用这个节点，因此当前线程就可以安全的回收这个节点的内存了。
    */

	struct node
	{
		std::shared_ptr<T> data;
		std::atomic<int> internal_count; // 内部计数器使用原子变量，因为内部计数器在节点内部，且这个点会被new出来，且被多个线程共享
		counted_node_ptr next; // 注意，这里next并不是指针，而是一个结构体，一个包含外部计数器和下一个node指针的结构体，并不是指针

		node(const T& data_) : data(std::make_shared<T>(data_)), internal_count(0) {}
	};

private:
	std::atomic<counted_node_ptr> head; // 注意，head此处不再是指针，而是结构体

private:
	// 进入pop函数的第一件事就是将head的外部计数器递增，表示当前有一个线程打算读取node指针
	void increase_head_count(counted_node_ptr& old_counter)
	{
		counted_node_ptr new_counter;
		do
		{
			// 此循环是为了能够确保head能被此线程获取并将其外部节点递增
			new_counter = old_counter;
			++new_counter.external_count;
		} while (!head.compare_exchange_strong(old_counter, new_counter)); // 注意，这里不再是对比指针，而是通过二进制方式对比结构体对象是否相等

		// 此处有可能发生线程切换，导致old_counter与当前真正的head不一致，不过没事，因为已经将之前的head的外部节点递增了，另一个线程可以负责处理那个head
		old_counter.external_count = new_counter.external_count;
	}

public:
	void push(const T& data)
	{
		counted_node_ptr new_node; // 这里并没有new出一个，直接创建一个栈对象
		new_node.ptr = new node(data); // 真正的节点内容
		new_node.external_count = 1; // 将外部节点初始化为1，因为当前有head在引用

		new_node.ptr->next = head.load();
		while (!head.compare_exchange_weak(new_node.ptr->next, new_node));
	}

	std::shared_ptr<T> pop()
	{
		counted_node_ptr old_head = head.load();
		while (true)
		{
			// 真正获取当前head，并将head的外部计数器递增1。注意，此时等此函数退出时，old_head并不一定等于当前stack的head
			increase_head_count(old_head);

			// 假如不在获取指针之前对外部计数器递增，则其他线程可能会释放这个ptr，导致当前线程的ptr变成了悬垂指针
			node* const ptr = old_head.ptr;
			if (!ptr)
			{
				return nullptr;
			}

			// 这里的if才是真正尝试将head从stack中移除，如果发现当前的head与刚才获取的old_head不一致，说明当前线程在
			// 获取old_head并将head的外部计数器递增1后，另一个线程将这个递增后的head给移除了，并没有轮到当前线程来处理。
			if (head.compare_exchange_strong(old_head, ptr->next))
			{
				// 如果发现head与old_head一致，那么就将head移除，然后将head更新为next。此时本线程就可以放心的处理old_head了。
				// 注意，即使当前线程进入到了这里，old_head对应的指针可能也被其他线程的old_head对象所引用，这个就看old_head
				// 里的外部计数器的值了。
				std::shared_ptr<T> res = nullptr;
				res.swap(ptr->data);

				// 如果其他线程并没有引用old_head中的node指针，则理论上old_head中的外部计数器的值是2，因为刚才在increase_head_count
				// 中对其进行了递增。如果此时其他线程也引用了old_head中对应的node指针，则此时old_head中的外部计数器的值一定大于2，且
				// 减去2之后的值就是其他线程引用的个数(或者你可以这么理解，由于当前线程将不再引用这个节点，因此要把外部计数器减去1。然后
				// 由于这个head节点已经从stack中移除了，所以stack不再引用这个节点了，因此外部引用计数又再次减去1)。然后就需要比较外部
				// 计数器与内部计数器之和是否为0。如果之和为0，则表示现在没有其他线程引用这个节点了，那么就可以安全的删除这个节点了。

				// 注意，如果它们之和为0，则表示internal_count之前的数值一定是-count_increase的值，因为fetch_add返回的是旧值，所以你
				// 会发现下面比较有点怪异，不是比较0，而是比较旧值与-count_increase。
				const int count_increase = old_head.external_count - 2;
				if (ptr->internal_count.fetch_add(count_increase) == -count_increase)
				{
					delete ptr;
				}

				return res;
			}
			else if (ptr->internal_count.fetch_sub(1) == 1)
			{
				// 不管是由于其他线程已经把这个节点的内容返回了，还是其他线程新加了节点到stack中，此时都要重新循环，从而重新得到新的head并pop。
				// 但是在重新循环之前，由于上面在获取head时已经将head外部计数器加1了，那么这里需要将内部计数器减去1，表示当前线程不再引用这个节
				// 点了。如果发现内部计数器减去1之后变成了0，则表示内部计数器之前是1，所以肯定有其他线程已经返回这个节点的内容了
                //   **只要确定内部计数器的值大于0，就表示肯定有其他线程已经进入了上面的if分支并且会把节点中的值返回**
                // 且如果正巧发现内部计数器的值为1，则表示当前已经没有其他线程再引用这个节点了(因为当前线程马上就要将内部计数器减1，则内部计数器就
                // 变成了0，就表示没有任何线程拥有这个节点了)。因此，此时就可以直接删除这个节点了。
				delete ptr;
			}
		}
	}

	~lock_free_stack()
	{
		while (pop());
	}
};

// 参考：https://blog.csdn.net/weixin_43376501/article/details/108325765
/*
因此，push()线程之间无需happens-before要求，push()和pop()之间通过操作3和操作4完成happens-before要求，
pop()与pop()之间通过操作9和操作12完成happens-before要求。

当有多个push()以及多个pop()线程同时执行时，我们只需要考虑push()与pop()之间的关系即可，且只需要考虑一个
push()对多个pop()的情况。因为如果多个pop()分别各自对应一个push()的话，那是没问题的，一对push()和pop()
并不会影响另一对push()和pop()。考虑一个push()对多个pop()时，由于操作4是RWM操作，因此此时多个pop之间的操作[4]
组成了release-sequence的一部分。根据release-sequence rule，这个push()肯定会happens-before最后成功的带head
并执行后续操作的那些pop()，因此也没有问题。

高性能需要依赖：硬件平台是否支持双字节比较并交换操作。
*/
template<typename T>
class lock_free_stack2
{
private:
	struct node;
	struct counted_node_ptr
	{
		int external_count = 0;
		node* ptr = nullptr;
	};

	struct node
	{
		std::shared_ptr<T> data;
		std::atomic<int> internal_count;
		counted_node_ptr next;

		node(const T& data_) : data(std::make_shared<T>(data_)), internal_count(0) {}
	};

private:
	std::atomic<counted_node_ptr> head;

private:
	void increase_head_count(counted_node_ptr& old_counter)
	{
		counted_node_ptr new_counter;
		do
		{
			new_counter = old_counter;
			++new_counter.external_count;
		} while (!head.compare_exchange_strong(old_counter, new_counter,
            std::memory_order_acquire, std::memory_order_relaxed)); // 4

		old_counter.external_count = new_counter.external_count; // 5
	}

public:
	void push(const T& data)
	{
		counted_node_ptr new_node;
		new_node.ptr = new node(data); // 1
		new_node.external_count = 1; // 2

		new_node.ptr->next = head.load(std::memory_order_relaxed); // 2.5
		while (!head.compare_exchange_weak(new_node.ptr->next, new_node,
            std::memory_order_release, std::memory_order_relaxed)); // 3
	}

	std::shared_ptr<T> pop()
	{
		counted_node_ptr old_head = head.load(std::memory_order_relaxed);
		while (true)
		{
			increase_head_count(old_head);
			node* const ptr = old_head.ptr; // 6
			if (!ptr)
			{
				return nullptr;
			}

			if (head.compare_exchange_strong(old_head, ptr->next,
                std::memory_order_relaxed)) // 7
			{
				std::shared_ptr<T> res = nullptr;
				res.swap(ptr->data); // 8

				const int count_increase = old_head.external_count - 2;
				if (ptr->internal_count.fetch_add(count_increase,
                    std::memory_order_release) == -count_increase) // 9
				{
					delete ptr; // 10
				}

				return res;
			}
			else if (ptr->internal_count.fetch_sub(1,
                std::memory_order_relaxed) == 1) // 11
			{
				ptr->internal_count.load(std::memory_order_acquire); // 12
				delete ptr; // 13
			}
		}
	}

	~lock_free_stack2()
	{
		while (pop());
	}
};

}


#endif