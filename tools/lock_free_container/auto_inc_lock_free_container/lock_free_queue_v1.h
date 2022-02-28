#ifndef _TOOLS_LOCK_FREE_CONTAINER_QUEUE_V1_H_
#define _TOOLS_LOCK_FREE_CONTAINER_QUEUE_V1_H_

#include <memory>
#include <cassert>

#include <functional>
#include <thread>
#include <atomic>

namespace tools
{

// spsc
// 7 和 1同步
// push 对 数据的存储，先于7
// pop  对 数据的加载晚于1
// 所以 加载晚于存储
template<typename T>
class lock_free_queue
{
private:
    struct node
    {
        std::shared_ptr<T> data;
        node* next;
        
        node():
        next(nullptr)
        {}
    };

    std::atomic<node*> head;
    std::atomic<node*> tail;

    // 7的加载和1的获取进行同步
    node* pop_head()
    {
        node* const old_head=head.load();
        if(old_head==tail.load())  // 1
        {
            return nullptr;
        }
        head.store(old_head->next);
        return old_head;
    }

public:
    lock_free_queue():
        head(new node),tail(head.load())
    {}

    lock_free_queue(const lock_free_queue& other)=delete;
    lock_free_queue& operator=(const lock_free_queue& other)=delete;

    ~lock_free_queue()
    {
        while(node* const old_head=head.load())
        {
            head.store(old_head->next);
            delete old_head;
        }
    }

    // 加载data 晚于加载tail指针
    std::shared_ptr<T> pop()
    {
        node* old_head=pop_head();
        if(!old_head)
        {
            return std::shared_ptr<T>();
        }

        std::shared_ptr<T> const res(old_head->data);  // 2
        delete old_head;
        return res;
    }

    // 尾部有一个空节点
    // 5对data的操作 先于 7对tail指针的操作
    void push(T new_value)
    {
        std::shared_ptr<T> new_data(std::make_shared<T>(new_value));
        node* p = new node;  // 3
        node* const old_tail=tail.load();  // 4
        old_tail->data.swap(new_data);  // 5
        old_tail->next = p;  // 6
        tail.store(p);  // 7
    }
};

}

#endif