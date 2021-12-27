#ifndef _TOOLS_ITERATOR_BASE_H_
#define _TOOLS_ITERATOR_BASE_H_

#include <memory>
#include <cassert>

namespace tools
{

#if 0
template<typename T, typename Reference = T&>
class Iterator
{
  typedef Iterator<T> _Self;
public:
    Iterator() {}
    virtual ~Iterator() {}

    virtual Reference operator*();
    virtual T* operator->();
    virtual _Self& operator++();
    virtual _Self operator++(int);

    virtual bool operator ==(const _Self& val);
    virtual bool operator !=(const _Self& val);
};

template<typename T, typename Reference = T&>
class ReverseIterator : public Iterator<T, Reference>
{
  typedef ReverseIterator<T> _Self;
public:
    ReverseIterator() : Iterator<T, Reference>() {}
    virtual ~ReverseIterator() {}

    virtual _Self& operator--();
    virtual _Self operator--(int);
};

template<typename T, typename Reference = T&, typename Distance = ptrdiff_t>
class RandomIterator : public ReverseIterator<T, Reference>
{
    typedef RandomIterator<T, Distance> _Self;
public:
    RandomIterator() : ReverseIterator<T, Reference>() {}
    ~RandomIterator() {}

    virtual _Self operator+(Distance n);
    virtual _Self& operator+=(Distance n);

    virtual _Self operator-(Distance n) = 0;
    virtual _Self& operator-=(Distance n) = 0;

    virtual Reference operator[](Distance n) = 0;

    virtual bool operator<(const _Self& val) = 0;
};

#endif

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag	: public input_iterator_tag {};
struct bidirectional_iterator_tag: public forward_iterator_tag {}; // 双向
struct random_access_iterator_tag: public bidirectional_iterator_tag {};

template<typename Category,typename _Ty,typename _Dis = int,
         typename Pointer = _Ty * , typename Reference = _Ty &>
struct iterator
{
	typedef Category  iterator_category;
	typedef _Ty       value_type;
	typedef _Dis      difference_type;
	typedef Pointer   pointer;
	typedef Reference reference;
};
template<typename _Ty,typename _Dis>
struct _Bidit: 
	public iterator<bidirectional_iterator_tag,_Ty,_Dis>
{};

template<typename _Ty, typename _Dis>
struct _Ranit : 
	public iterator<random_access_iterator_tag,	_Ty, _Dis>
{};



///////////////////////////
// todo
template<typename _Ty, typename _Dis = ptrdiff_t>
class RIterator : public _Ranit<_Ty, _Dis>
{
public:
    typedef _Dis            Distance;
    typedef _Ty*            _Ptr;
    typedef _Ty&            reference;
    typedef  const _Ty*     _Cptr;

public:
    RIterator(_Ty* ptr = nullptr): ptr_(ptr) {}

    reference operator*()
    {
        return *ptr_;
    }
    _Cptr operator->()
    {
        return &**this;
    }
    RIterator& operator++()
    {
        ++ ptr_;
        return *this;
    }
    RIterator operator++(int)
    {
        RIterator temp = *this;
        ++*this;
        return temp;
    }
    RIterator& operator--()
    {
        -- ptr_;
        return *this;
    }
    RIterator operator--(int)
    {
        RIterator temp = *this;
        ++*this;
        return temp;
    }
    RIterator operator+(Distance n) const
    {
        return ptr_ + n;
    }
    RIterator& operator+=(Distance n)
    {
        ptr_ = ptr_ + n;
        return *this;
    }
    RIterator operator-(Distance n)
    {
        return ptr_ - n;
    }

    Distance operator-(const RIterator& val) const
    {
        return ptr_ - val.ptr_;
    }

    RIterator& operator-=(Distance n)
    {
        ptr_ = ptr_ - n;
        return *this;
    }
    reference operator[](Distance n)
    {
        return *(ptr_ + n);
    }
    bool operator ==(const RIterator& val)
    {
        return ptr_ == val.ptr_;
    }
    bool operator !=(const RIterator& val)
    {
        return !(*this == val);
    }

    bool operator<(const RIterator& val)
    {
        return ptr_ < val.ptr_;
    }

    bool operator>(const RIterator& val)
    {
        return !(*this == val || *this < val); 
    }
protected:
  _Ty* ptr_;
};

}

#endif