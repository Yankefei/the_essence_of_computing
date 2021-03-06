#ifndef _TOOLS_VECTOR_H_
#define _TOOLS_VECTOR_H_

#include <memory>
#include <cassert>

#include "iterator_base.h"

namespace tools
{

template<typename T, typename Alloc = std::allocator<T>>
class Vector
{
public:
    typedef  T            value_type;
    typedef  T&           reference;
    typedef  const T&     const_reference;
    typedef  std::size_t  size_type;
    typedef  T*           _Ptr;
    typedef  const T*     _Cptr;

    typedef RIterator<T>  _It;

public:
    Vector() = default;

    Vector(std::initializer_list<T> list)
    {
        for(auto p = list.begin(); p != list.end(); p ++)
            emplace_back(*p);
    }

    ~Vector()
    {
        free();
    }

    Vector(const Vector& val)
    {
        auto new_data = alloc_n_copy(val.begin(), val.end());
        element_ = new_data.first;
        first_free_ = cap_ = new_data.second;
    }

    Vector(Vector&& val)  noexcept
    {
        set_status_null();
        swap_data(val);
    }

    Vector& operator=(const Vector&val)
    {
        auto data = alloc_n_copy(val.begin(), val.end());
        free();
        element_ = data.first;
        first_free_ = cap_ = data.second;
        return *this;
    }

    Vector& operator=(Vector&& val)  noexcept
    {
        if (this != &val)
        {
            free();
            set_status_null();
            swap_data(val);
        }
        return *this;
    }

    reference operator[](size_type n)
    {
        return element_[n];
    }

    const_reference operator[](size_type n) const
    {
        return element_[n];
    }

    _It begin() const
    {
        return _It(element_);
    }

    _It end() const
    {
        return _It(first_free_);
    }

    void push_back(const_reference val)
    {
        push_back(T(val));
    }

    void push_back(T&& val) noexcept
    {
        check_n_alloc();
        alloc_.construct(first_free_++, std::move(val));
    }

    template<typename... Args>
    void emplace_back(Args&&... args)  noexcept
    {
        check_n_alloc();
        alloc_.construct(first_free_++, std::forward<Args>(args)...);
    }

    void pop_back()
    {
        if (first_free_ == element_) return;
        alloc_.destroy(-- first_free_);
    }

    reference back()
    {
        return *(first_free_ - 1);
    }

    reference at(size_type n)
    {
        return element_ + n - 1;
    }

    void assign(size_type n, const_reference val)
    {
        while(n-- > 0)
            push_back(val);
    }

    value_type* data()
    {
        return element_;
    }

    void clear()
    {
        for (; first_free_ != element_; )
        {
            alloc_.destroy(-- first_free_);
        }
    }

    _It insert(_It pos, const_reference val);

    // ??? first ??? last ????????? *this ????????????????????????????????????
    _It insert(_It pos, _It first, _It last);

    _It erase(_It pos);

    _It erase(_It first, _It last);

    bool empty() const
    {
        return size() == 0;
    }

    size_type size() const
    {
        return first_free_ - element_;
    }

    size_type capacity() const
    {
        return cap_ - element_;
    }

    void resize(size_type count)
    {
        reallocate(count);
    }

    void reserve(size_type new_cap)
    {
        if (new_cap <= size()) return;

        reallocate(new_cap);
    }

private:
    void check_n_alloc()
    {
        if (size() == capacity())
            reallocate();
    }

    // ???????????????????????????????????????. ??????????????????????????????????????????????????????
    void reallocate(size_type _size = 0)
    {
        size_type newcapacity = 0;
        size_type orig_size = size();
        size_type cpy_data_size = orig_size;
        if (_size == 0)
        {
            newcapacity = orig_size ? 2 * orig_size : 1;
        }
        else
        {
            if (_size < cpy_data_size)
            {
                cpy_data_size = _size;
            }
            newcapacity = _size;
        }

        auto newdata = alloc_.allocate(newcapacity);
        auto dest = newdata;
        auto ele = element_;

        for (size_type i = 0; i < cpy_data_size; i++)
        {
            alloc_.construct(dest++, std::move(*ele ++));
        }
        free();
        element_ = newdata;
        first_free_ = dest;
        cap_ = element_ + newcapacity;
    }

    std::pair<value_type*, value_type*>
        alloc_n_copy(const _It begin, const _It end)
    {
        auto data = alloc_.allocate(end - begin);
        return {data, std::uninitialized_copy(begin, end, data)};
    }

    void free()
    {
        if (element_)
        {
            clear();
            alloc_.deallocate(element_, cap_ - element_);
        }
    }

    void set_status_null()
    {
        this->element_ = nullptr;
        this->first_free_ = nullptr;
        this->cap_ = nullptr;
    }

    void swap_data(Vector& x) noexcept
    {
        std::swap(this->element_, x.element_);
        std::swap(this->first_free_, x.first_free_);
        std::swap(this->cap_, x.cap_);
    }

private:
    // static std::allocator<T> alloc_; // static ??????

    // ???????????????????????????????????????std::allocator?????????????????????1??????????????????
    // ????????????????????????????????????????????????????????????????????????????????????????????????????????????8????????????
    // ????????????1?????????????????????????????????????????????????????? MString ???????????????????????????
    static Alloc        alloc_;
    // alloc ????????? __gnu_cxx::new_allocator<_Tp> ??????????????????????????????

    T* element_{nullptr};
    T* first_free_{nullptr};
    T* cap_{nullptr};
};


template<typename T, typename Alloc>
typename Vector<T, Alloc>::_It
Vector<T, Alloc>::insert(_It pos, const_reference val)
{
    if (pos < element_ || pos > first_free_) return pos;

    auto insert_index = pos - element_; // ???????????????????????????????????????????????????

    check_n_alloc();
    auto cpy_ptr = first_free_ - 1;
    alloc_.construct(first_free_++, T());  // ?????????????????????

    _Ptr dst = element_ + insert_index;

    for (auto ptr = cpy_ptr; ptr != dst - 1; ptr--)
    {
        *(ptr + 1) = *ptr;
    }
    *dst = val;

    return _It(dst);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::_It
Vector<T, Alloc>::insert(_It pos, _It first, _It last)
{
    if (pos < element_ || pos > first_free_) return pos;
    if (first > last || first == last) return last;

    auto insert_index = pos - element_;
    auto num = last - first;

    while(capacity() - size() < num)
        check_n_alloc();

    auto cpy_ptr =  first_free_ - 1;
    std::uninitialized_fill_n(first_free_, num, T()); // ?????????????????????
    first_free_ += num;

    _Ptr dst = element_ + insert_index;

    for (; cpy_ptr >= dst; cpy_ptr--)
    {
        *(cpy_ptr + num) = *cpy_ptr;  // ????????????
    }

    cpy_ptr = dst; // ????????????
    while(first < last)
    {
        *cpy_ptr++ = *first ++;   // ????????????
    }

    return _It(dst);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::_It
Vector<T, Alloc>::erase(_It pos)
{
    if (pos < element_ || pos > first_free_) return pos;

    _Ptr dst = element_ + (pos - element_);

    auto dst_t = dst;
    for (auto ptr = dst + 1; ptr != first_free_; ptr++, dst_t ++)
    {
        *dst_t = *ptr;
    }
    alloc_.destroy(-- first_free_);

    return _It(dst);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::_It
Vector<T, Alloc>::erase(_It first, _It last)
{
    if (first < element_ || last > first_free_) return last;
    if (first > last || first == last) return last;

    _Ptr dst = element_ + (first - element_);
    auto num = last - first;

    auto dst_t = dst;
    for (auto ptr = dst + num; ptr != first_free_; ptr++, dst_t ++)
    {
        *dst_t = *ptr;
    }

    while(num-- > 0)
    {
        alloc_.destroy(-- first_free_);
    }

    return _It(dst);
}

template<typename T, typename Alloc>
Alloc Vector<T, Alloc>::alloc_;

// template<typename T>
// std::allocator<T> Vector<T>::alloc_;

// Todo ?????????bool??????????????????

}

namespace std  // ?????????std??????????????????, ?????? std::swap
{
template<typename T>
inline void swap(tools::Vector<T>&& lhs, tools::Vector<T>&& rhs)
{
    tools::Vector<T> temp = std::forward<tools::Vector<T>>(lhs);
    lhs = std::forward<tools::Vector<T>>(rhs);
    rhs = std::move(temp);
}

}

#endif