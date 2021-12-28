#ifndef _TOOLS_STRING_H_
#define _TOOLS_STRING_H_

#include <memory>
#include <cassert>

#include "iterator_base.h"

namespace tools
{

// 分析过 std::string 的实现逻辑，发现内部实现过于复杂
// 这个版本就以 tools::Vector 和 std::vector 方案为蓝本进行优化实现

template<typename T, typename Alloc>
struct BString_Base
{
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<T>  _Tp_alloc_type;
    
    // alloc_traits.h中已经有过定义，这里重复是为了代码更清晰
    /*
    template<typename _Alloc>
    struct allocator_traits: xxx
    {
    ...
      template<typename _Tp>
	using rebind_alloc = __alloc_rebind<_Alloc, _Tp>;
          template<typename _Tp>
	using rebind_traits = allocator_traits<rebind_alloc<_Tp>>;
    ...
    };
    */
    // 实际使用的就是 rebind_traits 类中的成员和函数
    typedef typename std::allocator_traits<_Tp_alloc_type>
        rebind_traits;

    // same as
    // typedef typename std::allocator_traits<Alloc>::template
    //     rebind_traits<T>::pointer  pointer;
    //typedef typename std::allocator_traits<_Tp_alloc_type>::pointer pointer;
    typedef typename rebind_traits::pointer pointer;

    // 继承是为了将该类的指针作为allocate或deallocate的第一个参数
    struct _BString_impl : public _Tp_alloc_type
    {
        _BString_impl()
        : _Tp_alloc_type(), _m_start(), _m_finish(), _m_cap_()
        {}
        
        ~_BString_impl() {}

        _BString_impl(_Tp_alloc_type const& _a) noexcept
        : _Tp_alloc_type(_a), _m_start(), _m_finish(), _m_cap_()
        {}

        _BString_impl(_Tp_alloc_type&& _a) noexcept
        : _Tp_alloc_type(std::move(_a)),
          _m_start(), _m_finish(), _m_cap_()
        {}

        void _swap_data(_BString_impl& _x) noexcept
        {
            std::swap(_m_start, _x._m_start);
            std::swap(_m_finish, _x._m.finish);
            std::swap(_m_cap_, _x._m_cap_);
        }

        pointer _m_start;
        pointer _m_finish;
        pointer _m_cap_;
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

    ~BString_Base()
    {
        _m_free();
    }

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
        return _n != 0 ? rebind_traits::allocate(_m_impl, _n) : pointer();
    }

    void _m_deallocate(pointer _p, size_t _n)
    {
        if (_p)
            rebind_traits::deallocate(_m_impl, _p, _n);
    }

    void _m_free()
    {
        _m_deallocate(_m_impl._m_start, _m_size());
    }

    size_t _m_size()
    {
        return _m_impl._m_finish - _m_impl._m_start;
    }

    void _m_reallocate(size_t _size = 0)
    {
        size_t newcapacity = 0;
        size_t orig_size = _m_size();
        size_t cpy_data_size = orig_size;
        if (_size == 0)
        {
            newcapacity = orig_size ? 2 * orig_size : 1;
        }
        else
        {
            if (orig_size == _size) return;

            if (_size < cpy_data_size)
            {
                cpy_data_size = _size;
            }
            newcapacity = _size;
        }

        auto newdata = _m_allocate(newcapacity);
        auto dest = newdata;
        auto ele = _m_impl._m_start;

        for (size_t i = 0; i != cpy_data_size; i++)
        {
            _Tp_alloc_type::construct(dest++, std::move(*ele ++));
        }

        _m_free();
        _m_impl._m_start = newdata;
        _m_impl._m_finish = dest;
        _m_impl._m_cap_ = _m_impl._m_start + newcapacity;
    }

private:
    void _m_create_storage(size_t _n)
    {
        this->_m_impl._m_start = this->_m_allocate(_n);
        this->_m_impl._m_finish = this->_m_impl._m_start;
        this->_m_impl._m_cap_ = this->_m_impl._m_start + _n;
    }
};

/*
            public   protected  private
共有继承     public   protected   不可见
私有继承     private  private     不可见
保护继承     protected protected  不可见
(效果，将类的使用者无法访问基类内部变量，但派生类本身可见)
*/

// 使用Alloc模板参数，这样的最终目的还是调用了 std::allocator中的
// __gnu_cxx::new_allocator 构造器
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
        // 必须携带_Base的作用域符
        return _Base::_m_size();
    }

    size_type length()
    {
        return _Base::_m_size();
    }

    bool empty()
    {
        return _Base::_m_size() == 0;
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
    void check_n_alloc()
    {
        if (size() == capacity())
            _Base::_m_reallocate();
    }

    std::pair<T*, T*>
        alloc_n_copy(const _It begin, const _It end)
    {

    }

    _It uninitialized_copy(const _It begin, const _It end, pointer)
    {
        
    }
};


using String = BString<char>;

}

#endif