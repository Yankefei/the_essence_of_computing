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
        cap_ = rhs.cap_;

        copy_data(rhs);
    }

    RQueue& operator=(const RQueue& rhs)
    {
        if (this != &rhs)
        {
            free();

            element_ = new T[rhs.cap_];
            cap_ = rhs.cap_;

            copy_data(rhs);
        }

        return *this;
    }

    bool push(const T&val) { return push_back(val); }
    bool push_back(const T& val)
    {
        if (!can_add_back()) return false;

        *back_ = val;
        add_back();

        return true;        
    }

    bool pop(const T&val) { return pop_front(); }
    void pop_front()
    {
        if (!can_add_front()) return;

        add_front();        
    }

    T& front() const
    {
        return *front_;
    }

    T& back() const
    {
        return *sub_point(back_);
    }

    std::size_t size() const
    {
        return size_;
    }

    bool empty() const
    {
        return size_ == 0;
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
            cap_ = 0;
        }
    }

    void copy_data(const RQueue& rhs)
    {
        front_ = element_ + (rhs.front_ - rhs.element_);
        back_ = element_ + (rhs.back_ - rhs.element_);
        auto front_temp = front_;
        auto rhs_front_temp = rhs.front_;
        for (; size_ != rhs.size_; size_ ++)
        {
            *front_temp = *rhs_front_temp;
            front_temp = add_point(front_temp);
            rhs_front_temp = rhs.add_point(rhs_front_temp);   // rhs's  add_point
        }
        assert(front_temp == back_);
        assert(rhs_front_temp == rhs.back_);
    }

    T* add_point(T* pointer) const
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

    T* sub_point(T* pointer) const
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

// 环形队列，使用数组下标的方式实现, 实现简单了很多
template<typename T>
class RQueue2
{
public:
    RQueue2(std::size_t size) : cap_(size > MIN_QUEUE_NUM ? size : MIN_QUEUE_NUM)
    {
        element_ = new T[cap_];
        front_ = tail_ = 0;
    }

    ~RQueue2()
    {
        free();
    }

    RQueue2(const RQueue2& rhs)
    {
        element_ = new T[rhs.cap_];
        cap_ = rhs.cap_;

        copy_data(rhs);
    }

    RQueue2& operator=(const RQueue2& rhs)
    {
        if (this != &rhs)
        {
            free();

            element_ = new T[rhs.cap_];
            cap_ = rhs.cap_;

            copy_data(rhs);
        }

        return *this;
    }

    bool push(const T&val) { return push_back(val); }
    bool push_back(const T& val)
    {
        if (is_full()) return false;

        element_[tail_] = val;
        tail_ = (tail_ + 1) % cap_;
        size_ ++;
        return true;
    }

    bool pop(const T&val) { return pop_front(); }
    void pop_front()
    {
        if (empty()) return;

        front_ = (front_ + 1) % cap_;
        size_ --;
    }

    T& front() const
    {
        assert(size_ > 0);
        return element_[front_];
    }

    T& back() const
    {
        assert(size_ > 0);
        return element_[(tail_ + cap_ - 1) % cap_];
    }

    std::size_t size() const
    {
        return size_;
    }

    bool empty() const
    {
        return size_ == 0;
    }

private:
    bool is_full()
    {
        return cap_ == size_;
    }

    void free()
    {
        if (cap_)
        {
            delete[] element_;
            cap_ = size_ = front_ = tail_ = 0;
        }
    }

    void copy_data(const RQueue2& val)
    {
        front_ = val.front_;
        tail_ = val.tail_;
        for(auto i = front_; size_ != val.size_;
            size_ ++, i = (i + 1) % val.cap_)
        {
            element_[i] = val.element_[i];
        }
    }

private:
    std::size_t cap_{0};
    T* element_{nullptr};

    std::size_t size_{0};
    // 当两个下标重合时，如果size_为0则为空，非零则为满
    std::size_t front_{0};
    std::size_t tail_{0};
};

}

#endif 