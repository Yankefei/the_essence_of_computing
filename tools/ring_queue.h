#ifndef _TOOLS_RING_QUEUE_H_
#define _TOOLS_RING_QUEUE_H_

#include <memory>
#include <cassert>

namespace tools
{

#define MIN_QUEUE_NUM 64

// 环形队列
template<typename T>
class RQueue
{
public:
    RQueue(std::size_t size) : cap_(size > MIN_QUEUE_NUM ? size : MIN_QUEUE_NUM)
    {
        element_ = new T[cap_];
        front_ = back_ = element_;
    }

    ~RQueue()
    {
        free();
    }

    RQueue(const RQueue& rhs)
    {
        element_ = new T[rhs.cap_];
        cap_ = cap_;

        front_ = element_ + (rhs.front - rhs.element_);
        back_ = element_ + (rhs.back_ - rhs.element_);
        auto front_temp = front_;
        auto rhs_front_temp = rhs.front_;
        for (; size_ != rhs.size_; size_ ++)
        {
            *front_temp = *rhs_front_temp;
            front_temp = add_point(front_temp);
            rhs_front_temp = add_point(rhs_front_temp);
        }
    }

    RQueue& operator=(const RQueue& rhs)
    {
        if (this != &rhs)
        {
            free();

            element_ = new T[rhs.cap_];
            cap_ = cap_;

            front_ = element_ + (rhs.front - rhs.element_);
            back_ = element_ + (rhs.back_ - rhs.element_);
            auto front_temp = front_;
            auto rhs_front_temp = rhs.front_;
            for (; size_ != rhs.size_; size_ ++)
            {
                *front_temp = *rhs_front_temp;
                front_temp = add_point(front_temp);
                rhs_front_temp = add_point(rhs_front_temp);
            }
        }

        return *this;
    }

    bool push_back(const T& val)
    {
        if (!can_add_back()) return false;

        *back_ = val;
        add_back();

        return true;        
    }

    void pop_front()
    {
        if (!can_add_front()) return;

        add_front();        
    }

    T& front()
    {
        return *front_;
    }

    T& back()
    {
        return *sub_point(back_);
    }

    std::size_t size()
    {
        return size_;
    }

private:
    void free()
    {
        if (element_)
        {
            delete[] element_;
            element_ = nullptr;
            size_ = 0;
            front_ = nullptr;
            back_ = nullptr;   
        }
    }

    T* add_point(T* pointer)
    {
        if (pointer == cap_ + element_ - 1)
        {
            return element_;
        }
        else
        {
            return ++ pointer;
        }
    }

    T* sub_point(T* pointer)
    {
        if (pointer == element_)
            return element_ + cap_ - 1;
        else
            return -- pointer;
    }

    bool can_add_back()
    {
        if (front_ == back_ && size_ != 0)
            return false;
        return true;
    }

    void add_back()
    {
        size_ ++;
        back_ = add_point(back_);
    }

    bool can_add_front()
    {
        if (size_ == 0)
            return false;
        return true;
    }

    void add_front()
    {
        front_ = add_point(front_);
        size_ --;
    }

private:
    std::size_t cap_{0};
    T* element_{nullptr};

    // 当两个指针重合时，如果size_为0则为空，非零则为满
    std::size_t size_{0};
    T* front_{nullptr};
    T* back_{nullptr};
};

}


#endif 