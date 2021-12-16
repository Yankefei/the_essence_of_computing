#ifndef _TOOLS_QUEUE_H_
#define _TOOLS_QUEUE_H_

#include <memory>
#include <cassert>

namespace tools
{

// 起始位置设置一个空节点的自增长队列
template<typename T>
class NQueue
{
    struct Node
    {
        Node() = default;
        Node(const T& val) : data(val) {}

        Node* next{nullptr};
        T data;
    };

    // using Node =  NQueue::Node;
public:
    NQueue(): size_(0)
    {
        back_ = front_ = buy_node();
    }

    ~NQueue()
    {
        free();
        free_node(front_);
        front_ = back_ = nullptr;
    }

    NQueue(const NQueue& rhs)
    {
        copy_data(rhs);
    }

    NQueue& operator=(const NQueue& rhs)
    {
        if (this != &rhs)
        {
            free();
            copy_data(rhs);
        }

        return *this;
    }

    void push_back(const T& val)
    {
        Node* ptr = buy_node(val);
        back_->next = ptr;
        back_ = ptr;
        size_ ++;
    }

    void pop_front()
    {
        if (front_ == back_) return;

        pop();
        size_ --;
    }

    T& front() const
    {
        return front_->next->data;
    }

    T& back() const
    {
        return back_->data;
    }

    size_t size() const
    {
        return size_;
    }

private:
    void free()
    {
        while(front_->next != nullptr)
        {
            pop();
        }
        size_ = 0;
    }

    void copy_data(const NQueue& val)
    {
        back_ = front_ = buy_node();

        auto iter = val.front_->next;
        for (; iter != nullptr; iter = iter->next)
        {
            push_back(iter->data);
        }
    }

    void pop()
    {
        Node* p = front_->next;
        if (back_ == p)
        {
            back_ = front_;
        }
        front_->next = front_->next->next;
        free_node(p);
    }

    Node* buy_node(const T& val = T())
    {
        Node* ptr = new Node(val);
        return ptr;
    }

    void free_node(Node* p)
    {
        delete p;
    }

private:
    Node* front_{nullptr};
    Node* back_{nullptr};
    std::size_t size_{0};
};



// 起始位置没有空节点的自增长队列
template<typename T>
class Queue
{
    struct Node
    {
        Node() = default;
        Node(const T& val) : data(val) {}

        Node* next{nullptr};
        T data;
    };

    // using Node = Queue<T>::Node;
public:
    Queue() = default;

    ~Queue()
    {
        free();
    }

    Queue(const Queue& rhs)
    {
        copy_data(rhs);
    }

    Queue& operator=(const Queue& rhs)
    {
        if (this != &rhs)
        {
            free();
            copy_data(rhs);
        }
        return *this;
    }

    void push_back(const T& val)
    {
        Node* p = buy_node(val);

        if (back_ != nullptr)
            back_->next = p;
        back_ = p;

        if (front_ == nullptr)
            front_ = p;

        size_ ++;
    }

    void pop_front()
    {
        if (front_ == nullptr) return;

        Node* p = front_;
        front_ = front_->next;
        free_node(p);
        size_ --;
    }

    T& front() const
    {
        return front_->data;
    }

    T& back() const
    {
        return back_->data;
    }

    size_t size() const
    {
        return size_;
    }

private:
    void free()
    {
        while(front_ != nullptr)
        {
            Node* p = front_;
            front_ = front_->next;
            free_node(p);
        }
        size_ = 0;
        back_ = nullptr;
    }

    void copy_data(const Queue& rhs)
    {
        auto iter = rhs.front_;
        for (; iter != nullptr; iter = iter->next)
        {
            push_back(iter->data);
        }
    }

    Node* buy_node(const T& val)
    {
        Node* ptr = new Node(val);
        return ptr;
    }

    void free_node(Node* p)
    {
        delete p;
    }

private:
    Node* front_{nullptr};
    Node* back_{nullptr};
    std::size_t size_{0};
};

}

#endif
