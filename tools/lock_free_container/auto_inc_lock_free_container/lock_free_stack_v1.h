#ifndef _TOOLS_LOCK_FREE_CONTAINER_STACK_V1_H_
#define _TOOLS_LOCK_FREE_CONTAINER_STACK_V1_H_

#include <memory>
#include <cassert>

#include <functional>
#include <thread>
#include <atomic>

namespace tools
{

unsigned const max_hazard_pointers=100;
struct hazard_pointer
{
    std::atomic<std::thread::id> id;
    std::atomic<void*> pointer;
};
hazard_pointer hazard_pointers[max_hazard_pointers];

class hp_owner
{
    hazard_pointer* hp;

public:
    hp_owner(hp_owner const&)=delete;
    hp_owner operator=(hp_owner const&)=delete;
    hp_owner():
        hp(nullptr)
    {
        for(unsigned i=0; i<max_hazard_pointers; ++i)
        {
            std::thread::id old_id;
            if(hazard_pointers[i].id.compare_exchange_strong(  // 6 尝试声明风险指针的所有权
                old_id,std::this_thread::get_id()))
            {
                hp=&hazard_pointers[i];
                break;  // 7
            }
        }
        if(!hp)  // 1
        {
            throw std::runtime_error("No hazard pointers available"); // 线程数超过最大限度
        }
    }

    std::atomic<void*>& get_pointer()
    {
        return hp->pointer;
    }

    ~hp_owner()  // 2
    {
        hp->pointer.store(nullptr);  // 8
        hp->id.store(std::thread::id());  // 9
    }
};

std::atomic<void*>& get_hazard_pointer_for_current_thread()  // 3
{
    thread_local static hp_owner hazard;  // 4 每个线程都有自己的风险指针，线程退出时才会被销毁
    return hazard.get_pointer();  // 5
}

bool outstanding_hazard_pointers_for(void* p)
{
    for(unsigned i=0;i<max_hazard_pointers;++i)
    {
        if(hazard_pointers[i].pointer.load()==p)
        {
            return true;
        }
    }
    return false;
}


template<typename T>
void do_delete(void* p)
{
    delete static_cast<T*>(p);
}

struct data_to_reclaim
{
    void* data;
    std::function<void(void*)> deleter;
    data_to_reclaim* next;

    // 模板化构造函数
    template<typename T>
    data_to_reclaim(T* p):  // 1
        data(p),
        deleter(&do_delete<T>),
        next(0)
    {}

    ~data_to_reclaim()
    {
        deleter(data);  // 2
    }
};

// 新的回收列表
std::atomic<data_to_reclaim*> nodes_to_reclaim;

void add_to_reclaim_list(data_to_reclaim* node)  // 3
{
    node->next=nodes_to_reclaim.load();
    while(!nodes_to_reclaim.compare_exchange_weak(node->next,node));
}

template<typename T>
void reclaim_later(T* data)  // 4
{
    add_to_reclaim_list(new data_to_reclaim(data));  // 5
}

// 这里的处理，如果有两个线程同时访问，第一个将获取到 nodes_to_reclaim中的头指针来进行处理
void delete_nodes_with_no_hazards()
{
    data_to_reclaim* current = nodes_to_reclaim.exchange(nullptr);  // 6 关键且简单，保证只有一个线程回收这些节点
    while(current)
    {
        data_to_reclaim* const next=current->next;
        if(!outstanding_hazard_pointers_for(current->data))  // 7
        {
            delete current;  // 8
        }
        else
        {
            add_to_reclaim_list(current);  // 9
        }
        current=next;
    }
}

// 使用风险指针的方式回收内存的版本
template<typename T>
class lock_free_stack
{
private:
    struct node
    {
        std::shared_ptr<T> data;  // 1 指针获取数据
        node* next;

        node(T const& data_):
        data(std::make_shared<T>(data_))  // 2 让std::shared_ptr指向新分配出来的T
        {}
    };

    std::atomic<node*> head;
    std::atomic<unsigned> threads_in_pop;
    std::atomic<node*> to_be_deleted;     // 多线程pop的一个处理分界点

public:
    // 先把准备工作做好，然后集中处理临界区问题
    void push(T const& data)
    {
        node* const new_node = new node(data);
        new_node->next=head.load();
        while(!head.compare_exchange_weak(new_node->next,new_node));
    }

    /*
       当线程想要删除一个对象，那么它就必须检查系统中其他线程是否持有风险指针。当没有风险指针的时候，
       那么它就可以安全删除对象。否则，它就必须等待风险指针的消失了。这样，线程就得周期性的检查其想
       要删除的对象是否能安全删除。
       应该是一种更简单的处理方案
    */
    std::shared_ptr<T> pop()
    {
        std::atomic<void*>& hp = get_hazard_pointer_for_current_thread();
        node* old_head=head.load();
        do
        {
            node* temp;
            // 在while循环中就能保证node不会在读取旧head指针 1 时，以及在设置风险指针的时被删除了
            do  // 1 直到将风险指针设为head指针
            {
                temp=old_head;
                hp.store(old_head);
                old_head=head.load();  // 再次获取head指针的值。避免多线程影响。如果有其他线程通过了下面的compare_exchange函数，就需要重新获取
            } while(old_head!=temp);
        }

        // 处理分界线
        while(old_head &&
            !head.compare_exchange_strong(old_head,old_head->next));  // 使用strong版本，因为weak可能导致old_head被重置，没有必要

        hp.store(nullptr);  // 2 当声明完成，清除风险指针
        std::shared_ptr<T> res;
        if(old_head)
        {
            res.swap(old_head->data);
            if(outstanding_hazard_pointers_for(old_head))  // 3 在删除之前对风险指针引用的节点进行检查
            {
                reclaim_later(old_head);  // 4
            }
            else
            {
                delete old_head;  // 5
            }
            // 最终的守门员函数, 最后一执行到的线程必然能够将之前的所有待清理的节点全部清理
            delete_nodes_with_no_hazards();  // 6  
        }
        return res;
    }

#if 0
    static void delete_nodes(node* nodes)
    {
        while(nodes)
        {
            node* next = nodes->next;
            delete nodes;
            nodes = next;
        }
    }

    // 垃圾回收机制
    // 最简单的替换机制就是使用*风险指针*(hazard pointer)
    void try_reclaim(node* old_head)
    {
        if(threads_in_pop == 1)  // 1
        {
            node* nodes_to_delete = to_be_deleted.exchange(nullptr);  // 2 声明“可删除”列表
            if(!--threads_in_pop)  // 3 是否只有一个线程调用pop()？
            {
                delete_nodes(nodes_to_delete);  // 4
            }
            else if(nodes_to_delete)  // 5  说明这时有其他线程也在pop, 而且修改了to_be_deleted的内容。
            {
                chain_pending_nodes(nodes_to_delete);  // 6
            }
            delete old_head;  // 7
        }
        else
        {
            // 这里将
            chain_pending_node(old_head);  // 8
            --threads_in_pop;
        }
    }

    void chain_pending_nodes(node* nodes)
    {
        node* last = nodes;
        while(node* const next = last->next)  // 9 让next指针指向链表的末尾
        {
            last = next;
        }
        chain_pending_nodes(nodes,last);
    }

    // 作用：添加first-last这段内容到to_be_deleted之前，并更换to_be_deleted的内容
    void chain_pending_nodes(node* first, node* last)
    {
        last->next = to_be_deleted;  // 10
        while(!to_be_deleted.compare_exchange_weak(  // 11 用循环来保证last->next的正确性
            last->next, first));
    }

    void chain_pending_node(node* n)
    {
        chain_pending_nodes(n, n);  // 12
    }
#endif
};

}

#endif