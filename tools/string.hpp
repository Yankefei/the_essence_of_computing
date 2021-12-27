#ifndef _TOOLS_STRING_H_
#define _TOOLS_STRING_H_

#include <memory>
#include <cassert>

#include "iterator_base.h"

namespace tools
{

// 分析过 std::string的实现逻辑，发现内部实现过于复杂
// 这个版本就以 tools::Vector 和 std::vector 方案为蓝本进行优化实现

template<typename T, typename Alloc>
struct BString_Base
{
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<T>  _Tp_alloc_type;

    typedef typename std::allocator_traits<_Tp_alloc_type>::pointer
        pointer;
    
    struct _BString_impl : public _Tp_alloc_type
    {
        _BString_impl()
        : _Tp_alloc_type(), _m_start(), _m_finish(), _m_end_of_storage()
        {}
        
        ~_BString_impl() {}

        _BString_impl(_Tp_alloc_type const& _a) noexcept
        : _Tp_alloc_type(_a), _m_start(), _m_finish(), _m_end_of_storage()
        {}

        _BString_impl(_Tp_alloc_type&& _a) noexcept
        : _Tp_alloc_type(std::move(_a)),
          _m_start(), _m_finish(), _m_end_of_storage()
        {}

        void _swap_data(_BString_impl& _x) noexcept
        {
            std::swap(_m_start, _x._m_start);
            std::swap(_m_finish, _x._m.finish);
            std::swap(_m_end_of_storage, _x._m_end_of_storage);
        }

        pointer _m_start;
        pointer _m_finish;
        pointer _m_end_of_storage;
    };

public:
    typedef Alloc allocator_type;

    _Tp_alloc_type& _get_Tp_allocator() noexcept
    {
        return *static_cast<_Tp_alloc_type*>(&this->_m_impl);
    }

    const _Tp_alloc_type& _get_Tp_allocator() const noexcept
    {
        return *static_cast<_Tp_alloc_type*>(&this->_m_impl);
    }

    allocator_type
    get_allocator() const noexcept
    {
        return allocator_type(_get_Tp_allocator());
    }

    BString_Base() : _m_impl() {}
    ~BString_Base() {}

    BString_Base(const allocator_type& _a) noexcept
    : _m_impl(_a) {}

    BString_Base(size_t _n)
    : _m_impl()
    { _m_create_storage(_n); }

    BString_Base(_Tp_alloc_type&& _a) noexcept
    : _m_impl(std::move(_a)) {}

    BString_Base(BString_Base&& _x) noexcept
    : _m_impl(std::move(_x._get_Tp_allocator()))
    {
        this->_m_impl._swap_data(_x._m_impl);
    }

    // 考虑到使用两个构造器的场景
    BString_Base(BString_Base&& _x, const allocator_type& _a) noexcept
    : _m_impl(_a)
    {
        if (_x.get_allocator() == _a)
            this->_m_impl._swap_data(_x._m_impl);
        else
        {
            size_t _n = _x._m_impl._m_finish - _x._m_impl._m_start;
            _m_create_storage(_n);
        }
    }

public:
    _BString_impl   _m_impl;

    pointer _m_allocate(size_t _n)
    {
        return _n != 0 ? _Tp_alloc_type::allocate(_m_impl, _n) : pointer();
    }

    void _m_deallocate(pointer _p, size_t _n)
    {
        if (_p)
            _Tp_alloc_type::deallocate(_m_impl, _p, _n);
    }

    void check_n_alloc()
    {

    }

    void _m_reallocate(size_t _size = 0)
    {}

private:
    void _m_create_storage(size_t _n)
    {
        this->_m_impl._m_start = this->_m_allocate(_n);
        this->_m_impl._m_finish = this->_m_impl._m_start;
        this->_m_impl._m_end_of_storage = this->_m_impl._m_start + _n;
    }
};

/*
            public   protected  private
共有继承     public   protected   不可见
私有继承     private  private     不可见
保护继承     protected protected  不可见
(效果，将类的使用者无法访问基类内部变量，但派生类本身可见)
*/

template<typename T, typename Alloc = std::allocator<T>>
class BString : protected BString_Base<T, Alloc>
{
    typedef BString_Base<T, Alloc>			_Base;
    typedef typename _Base::_Tp_alloc_type  _Tp_alloc_type;

public:
    typedef typename _Base::pointer         pointer;
    typedef T                               value_type;
    typedef T*                              _Ptr;
    typedef const T*                        _Cptr;
    typedef std::size_t                     size_type;
    typedef ptrdiff_t                       difference_type;
    typedef RIterator<T>                    _It;

    static const size_type npos = static_cast<size_type>(-1); 

protected:
    using _Base::_m_impl;

public:
    BString() {}
    ~BString() {}

    BString(std::initializer_list<value_type> list)
    {
        // for(auto p = list.begin(); p != list.end(); p ++)
        //     emplace_back(*p);
    }

    BString(const BString& rhs)
    {}

    BString& operator=(const BString& rhs)
    {}

    BString(BString&& rhs)
    {}

    BString& operator=(BString&& rhs)
    {}

    size_type size()
    {

    }

    size_type length()
    {
        return size();
    }

    bool empty()
    {
        return size() == 0;
    }

    size_type capacity() const
    {
    }

    void resize(size_type count)
    {
        // reallocate(count);
    }

    void reserve(size_type new_cap)
    {
        // if (new_cap <= size()) return;

        // reallocate(new_cap);
    }

    _It insert(_It pos, value_type val);

    // 若 first 和 last 是指向 *this 中的迭代器，则行为未定义
    _It insert(_It pos, _It first, _It last);

    _It erase(_It pos);

    _It erase(_It first, _It last);

    _Cptr* c_str()
    {
    }

    _Cptr* data()
    {}

    value_type operator[](size_type)
    {

    }

    value_type front() const
    {}

    value_type back() const
    {}

    BString& append(BString& rhs)
    {
        return *this;
    }

    void clear()
    {
    }

    void push_back(value_type val)
    {}

    void pop_back()
    {}

    void swap(BString& val)
    {

    }

    static void swap(BString& lhs, BString& rhs)
    {

    }


    size_type find( const BString& str, size_type pos = 0 ) const;

    size_type find( value_type ch, size_type pos = 0 ) const;

private:
    std::pair<T*, T*>
        alloc_n_copy(const _It begin, const _It end)
    {

    }

};


using String = BString<char>;

}

#endif