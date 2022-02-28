#include <iostream>
#include "stream.h"

#include <thread>
#include "lock_free_container/auto_inc_lock_free_container/lock_free_stack_v2.h"
#include <memory>

using namespace tools;

template<typename T>
struct node;

// std::atomic 是否可以无锁化？ 不能
template<typename T>
struct counted_node_ptr
{
    int external_count = 0; 
    node<T>* ptr = nullptr;
};

template<typename T>
struct node
{
    std::shared_ptr<T> data;
    std::atomic<int> internal_count; // 内部计数器使用原子变量，因为内部计数器在节点内部，且这个点会被new出来，且被多个线程共享
    counted_node_ptr<T> next; // 注意，这里next并不是指针，而是一个结构体，一个包含外部计数器和下一个node指针的结构体，并不是指针

    node(const T& data_) : data(std::make_shared<T>(data_)), internal_count(0) {}
};

// std::atomic 是否可以无锁化？ 不能
struct test_node
{
    int external_count = 0; 
    void * ptr;
};

int main()
{
    stream << Boolalpha;
    std::atomic<counted_node_ptr<int>>  test_node_a1;
    stream << test_node_a1.is_lock_free() << std::endl;


    std::atomic<test_node>  test_node_a2;
    stream << test_node_a2.is_lock_free() << std::endl;

    return 0;
}