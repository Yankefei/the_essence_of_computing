#ifndef _TOOLS_ITERATOR_BASE_H_
#define _TOOLS_ITERATOR_BASE_H_

#include <memory>
#include <cassert>

namespace tools
{

template<typename T, typename Reference = T&>
class Iterator
{
  typedef Iterator<T> _Self;
public:
    Iterator() {}
    virtual ~Iterator() {}

    virtual Reference operator*() = 0;
    virtual T* operator->() = 0;
    virtual _Self& operator++() = 0;
    virtual _Self operator++(int) = 0;

    virtual bool operator ==(const _Self& val) = 0;
    virtual bool operator !=(const _Self& val) = 0;
};

template<typename T, typename Reference = T&>
class ReverseIterator : public Iterator<T, Reference>
{
  typedef ReverseIterator<T> _Self;
public:
    ReverseIterator() : Iterator<T, Reference>() {}
    virtual ~ReverseIterator() {}

    virtual _Self& operator--() = 0;
    virtual _Self operator--(int) = 0;
};

template<typename T, typename Reference = T&, typename Distance = ptrdiff_t>
class RandomIterator : public ReverseIterator<T, Reference>
{
    typedef RandomIterator<T, Distance> _Self;
public:
    RandomIterator() : ReverseIterator<T, Reference>() {}
    ~RandomIterator() {}

    virtual _Self operator+(Distance n) = 0;
    virtual _Self& operator+=(Distance n) = 0;

    virtual _Self operator-(Distance n) = 0;
    virtual _Self& operator-=(Distance n) = 0;

    virtual Reference operator[](Distance n) = 0;

    virtual bool operator<(const _Self& val) = 0;
};

}

#endif