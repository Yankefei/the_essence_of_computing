#ifndef _TOOLS_STACK_H_
#define _TOOLS_STACK_H_

#include <memory>
#include <cassert>

namespace tools
{

template<typename T>
class Stack
{
public:
    Stack() = default;
    ~Stack()
    {
        free();
    }

    Stack(const Stack& val)
    {
        auto new_data = alloc_n_copy(val.begin(), val.end());
        element_ = new_data.first;
        first_free_ = cap_ = new_data.second;
    }

    Stack& operator=(const Stack& val)
    {
        auto data = alloc_n_copy(val.begin(), val.end());
        free();
        element_ = data.first;
        first_free_ = cap_ = data.second;
        return *this;
    }

    void push(const T& val)
    {
        push(T(val));
    }

    void push(T&& val)   noexcept
    {
        check_n_alloc();
        alloc_.construct(first_free_++, std::move(val));
    }

    T& top()
    {
        assert(size() != 0);
        return *(first_free_ - 1);
    }

    bool pop()
    {
        if (size() == 0) return false;
        alloc_.destroy(-- first_free_);
        
    }

    bool empty() const
    {
        return size() == 0;
    }

    void clear()
    {
        for (; first_free_ != element_; )
        {
            alloc_.destroy(-- first_free_);
        }
    }

    std::size_t size() const
    {
        return first_free_ - element_;
    }

    std::size_t capacity() const
    {
        return cap_ - element_;
    }

private:
    T* begin() const { return element_; }
    T* end() const { return first_free_;}

    void check_n_alloc()
    {
        if (size() == capacity())
            reallocate();
    }

    void reallocate()
    {
        auto newcapacity = size() ? 2 * size() : 1;
        auto newdata = alloc_.allocate(newcapacity);
        auto dest = newdata;
        auto ele = element_;
        for (;ele != first_free_;)
        {
            alloc_.construct(dest++, std::move(*ele ++));
        }
        free();
        element_ = newdata;
        first_free_ = dest;
        cap_ = element_ + newcapacity;
    }

    std::pair<T*, T*> alloc_n_copy(const T* begin, const T* end)
    {
        auto data = alloc_.allocate(end - begin);
        return {data, std::uninitialized_copy(begin, end, data)}; // 刚好用完
    }

    void free()
    {
        if (element_)
        {
            for (auto p = first_free_; p != element_; )
            {
                alloc_.destroy(--p);
            }
            alloc_.deallocate(element_, cap_ - element_);
        }
    }

private:
    static std::allocator<T> alloc_;  // static 静态

    T* element_{nullptr};
    T* first_free_{nullptr};
    T* cap_{nullptr};
};

template<typename T>
std::allocator<T> Stack<T>::alloc_;

}


#endif 