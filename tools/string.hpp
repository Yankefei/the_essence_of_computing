#ifndef _TOOLS_STRING_H_
#define _TOOLS_STRING_H_

#include <memory>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdarg>

#include "stream.h"

#include "iterator_base.h"
#include "algorithm.hpp"

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
        : _Tp_alloc_type(), _m_start(), _m_finish(), _m_cap()
        {}
        
        ~_BString_impl() {}

        _BString_impl(_Tp_alloc_type const& _a) noexcept
        : _Tp_alloc_type(_a), _m_start(), _m_finish(), _m_cap()
        {}

        _BString_impl(_Tp_alloc_type&& _a) noexcept
        : _Tp_alloc_type(std::move(_a)),
          _m_start(), _m_finish(), _m_cap()
        {}

        void _swap_data(_BString_impl& _x) noexcept
        {
            std::swap(_m_start, _x._m_start);
            std::swap(_m_finish, _x._m_finish);
            std::swap(_m_cap, _x._m_cap);
        }

        pointer _m_start;
        pointer _m_finish;
        pointer _m_cap;
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
        // add \0
        return _n != 0 ? rebind_traits::allocate(_m_impl, _n + 1) : pointer();
    }

    void _m_deallocate(pointer _p, size_t _n)
    {
        if (_p)
            rebind_traits::deallocate(_m_impl, _p, _n + 1);
    }

    void _m_free()
    {
        _m_deallocate(_m_impl._m_start, _m_size());
        _m_impl._m_start = nullptr;
        _m_impl._m_finish = nullptr;
        _m_impl._m_cap = nullptr;
    }

    // 适用于 string 容器的自增长逻辑
    // 默认将容积扩大一倍
    // 否则将容积修改到参数制定的大小
    void _m_reallocate(size_t _size = 0)
    {
        size_t newcapacity = 0;
        size_t orig_size = _m_impl._m_cap - _m_impl._m_start;
        size_t cpy_data_size = _m_size();
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

        auto newdata = _m_allocate(newcapacity);
        memcpy(newdata, _m_impl._m_start, cpy_data_size);
        _m_free();

        _m_impl._m_start = newdata;
        _m_impl._m_finish = newdata + cpy_data_size;
        _m_add_str_end();
        _m_impl._m_cap = _m_impl._m_start + newcapacity;
    }

    void _m_add_str_end()
    {
        *_m_impl._m_finish = '\0';
    }

    void _m_add_finish_ptr(size_t n = 1)
    {
        _m_impl._m_finish += n;
    }

    void _m_sub_finish_ptr(size_t n = 1)
    {
        _m_impl._m_finish -= n;
    }

private:
    size_t _m_size()
    {
        return this->_m_impl._m_finish - _m_impl._m_start;
    }

    void _m_create_storage(size_t _n)
    {
        this->_m_impl._m_start = this->_m_allocate(_n);
        this->_m_impl._m_finish = this->_m_impl._m_start;
        this->_m_impl._m_cap = this->_m_impl._m_start + _n;
        _m_add_str_end();
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
// 如果使用自定义的Alloc, 本方法可支持内部逻辑融洽, 且支持默认构造器和自定义构造器
// 的相互赋值和构造
template<typename T, typename Alloc = std::allocator<T>>
class BString : protected BString_Base<T, Alloc>
{
    typedef BString_Base<T, Alloc>			_Base;
    typedef typename _Base::_Tp_alloc_type  _Tp_alloc_type;

public:
    typedef typename _Base::pointer         pointer;
    typedef T                               value_type;
    typedef T&                              value_reference;
    typedef T*                              _Ptr;
    typedef const T*                        _Cptr;
    typedef std::size_t                     size_type;
    typedef ptrdiff_t                       difference_type;
    typedef RIterator<T>                    _It;
    typedef CRIterator<T>                   _CIt;

    static const size_type npos = static_cast<size_type>(-1); 

protected:
    using _Base::_m_impl;

public:
    BString() {}

    ~BString()
    {
        _Base::_m_free();
    }

    BString(std::initializer_list<value_type> list)
    {
        for(auto p = list.begin(); p != list.end(); p ++)
            push_back(*p);
    }

    BString(_Cptr data)
        : BString(data, strlen(data))
    {
        // 构造函数中不能调用构造函数。。
        // size_type len = strlen(data);
        // BString(data, len);
    }

    BString(_Cptr data, size_type len)
        : BString(data, data + len)
    {
    }

    BString(_Cptr data, _Cptr end_data)
    {
        auto pair = alloc_n_copy(data, end_data);
        _m_impl._m_start = pair.first;
        _m_impl._m_finish = _m_impl._m_cap = pair.second;
        _Base::_m_add_str_end();
    }

    BString(const BString& rhs)
    {
        if (!rhs.empty())
        {
            auto pair = alloc_n_copy(rhs.cbegin(), rhs.cend());
            _m_impl._m_start = pair.first;
            _m_impl._m_finish = _m_impl._m_cap = pair.second;
            _Base::_m_add_str_end();
        }
    }

    BString(BString&& rhs)  noexcept
    {
        swap(rhs);
    }

    BString& operator=(const BString& rhs)
    {
        if (this != &rhs)
        {
            _Base::_m_free();
            auto pair = alloc_n_copy(rhs.cbegin(), rhs.cend());
            _m_impl._m_start = pair.first;
            _m_impl._m_finish = _m_impl._m_cap = pair.second;
            _Base::_m_add_str_end();
        }
        return *this;
    }


    BString& operator=(BString&& rhs)  noexcept
    {
        if (this != &rhs)
        {
            swap(rhs);
        }

        return *this;
    }

    size_type size() const
    {
        return _size();
    }

    size_type length() const
    {
        return _size();
    }

    bool empty() const
    {
        return _size() == 0;
    }

    size_type capacity() const
    {
        return _capacity();
    }

    void resize(size_type count)
    {
        return _Base::_m_reallocate(count);
    }

    void reserve(size_type new_cap)
    {
        if (new_cap <= size()) return;

        _Base::_m_reallocate(new_cap);
    }

    _Cptr c_str() const
    {
        return _data();
    }

    _Cptr data() const
    {
        return c_str();
    }

    value_reference operator[](size_type n)
    {
        return *(_data() + n);
    }

    value_type front() const
    {
        return *_data();
    }

    value_type back() const
    {
        return *(_data + size() - 1);
    }

    BString& append(const BString& rhs)
    {
        _append(end(), rhs.data(), rhs.size());
        return *this;
    }

    BString& append(_Cptr data)
    {
        size_type data_len = strlen(data);
        _append(end(), data, data_len);
        return *this;
    }

    BString& append(const value_type& data)
    {
        _append(end(), data);
        return *this;
    }

    void clear()
    {
        _m_impl._m_finish = _m_impl._m_start;
        _Base::_m_add_str_end();
    }

    void push_back(const value_type& val)
    {
        append(val);
    }

    void pop_back()
    {
        if (_size() > 0)
        {
            _Base::_m_sub_finish_ptr();
            _Base::_m_add_str_end();
        }
    }

    void swap(BString& val)
    {
        _m_impl._swap_data(val._m_impl);
    }

    static void swap(BString& lhs, BString& rhs)
    {
        std::swap(lhs._m_impl._m_start, rhs._m_impl._m_start);
        std::swap(lhs._m_impl._m_finish, rhs._m_impl._m_finish);
        std::swap(lhs._m_impl._m_cap, rhs._m_impl._m_cap);
    }

    _It begin()
    {
        return _It(_data());
    }

    _It end()
    {
        return _It(_data() + _size());
    }

    _CIt cbegin() const
    {
        return _CIt(_data());
    }

    _CIt cend() const
    {
        return _CIt(_data() + _size());
    }

    // 在index的位置插入
    BString& insert(size_type index, value_type val)
    {
        if (index > size() + 1) return *this;

        _append(begin() + index, val);
        return *this;
    }

    BString& insert(size_type index, const BString& val)
    {
        if (index > size() + 1) return *this;

        _append(begin() + index, val.c_str(), val.size());
        return *this;
    }

    // 若 first 和 last 是指向 *this 中的迭代器，则行为未定义
    _It insert(_It pos, _CIt first, _CIt last)
    {
        if (pos > end() + 1 || pos < begin()) return pos;

        size_type begin_index = pos - begin();
        _append(pos, &*first, last - first);

        return begin() + begin_index;
    }

    _It erase(_It pos);

    // 左闭右开
    _It erase(_It first, _It last);

    // kmp 算法
    size_type find( const BString& str, size_type pos = 0 ) const
    {
        if (pos >= size()) return pos;

        return alg::kmp_find<value_type>(this->_data() + pos, this->size() - pos,
                                              str.c_str(), str.size());
        // return alg::kmp_find2<value_type>(this->_data() + pos, this->size() - pos,
        //                                       str.c_str(), str.size());
    }

    // 暴力匹配
    // https://zhuanlan.zhihu.com/p/83334559
    size_type find_fd( const BString& str, size_type pos = 0 ) const
    {
        if (pos >= size()) return pos;

        size_type find = npos;

        _Cptr src = this->_data() + pos;
        _Cptr dst = str.c_str();

        size_type src_len = this->length() - pos;
        size_type dst_len = str.length();
        for (int i = 0; i <= src_len - dst_len; i ++)
        {
            int j;
            for (j = 0; j < dst_len; j++)
            {
                if (dst[j] != src[i + j])
                    break;
            }

            // 全部能匹配上
            if (j == dst_len)
            {
                find = i;
                break;
            }
        }

        return find;
    }

    size_type find( value_type ch, size_type pos = 0 ) const
    {
        if (pos >= size()) return pos;

        size_type find = npos;

        auto it = cbegin() + pos;
        for (; it != cend(); it ++)
        {
            if (*it == ch)
            {
                find = it - cbegin();
                break;
            }
        }

        return find;
    }


    BString& operator+=(const BString& str)
    {
        return this->append(str);
    }

    BString& operator+=(_Cptr ptr)
    {
        return this->append(ptr);
    }

    BString& operator+=(const value_type& c)
    {
        return this->append(c);
    }

    int compare(const BString& val) const
    {
        int res = 0;
        auto it = cbegin();
        auto v_it = val.cbegin();
        while(it != cend() && v_it != val.cend())
        {
            if (*it != *v_it)
            {
                return *it < *v_it ? -1 : 1;
            }
            else
            {
                it ++;
                v_it ++;
            }
        }

        if (v_it != val.cend()) return -1;

        if (it != cend()) return 1;

        return 0;
    }

    void print() const
    {
        stream << *this << std::endl;
    }

private:
    size_t _size() const
    {
        return _m_impl._m_finish - _m_impl._m_start;
    }

    size_t _capacity() const
    {
        return _m_impl._m_cap - _m_impl._m_start;
    }

    value_type* _data() const
    {
        return static_cast<value_type*>(_m_impl._m_start);
    }

    void _append(_It pos, _Cptr ptr, size_type n)
    {
        size_type begin_num = pos - begin();
        // 可能导致pos失效
        check_n_alloc(n);
        
        auto e_it = begin() + begin_num - 1;
        for (auto it = end() - 1; it != e_it; it --)
            *(it + n) = *it;

        memcpy(&*(begin() + begin_num), ptr, n);
        _Base::_m_add_finish_ptr(n);
        _Base::_m_add_str_end();
    }

    void _append(_It pos, const value_type& val)
    {
        size_type begin_num = pos - begin();
        // 可能导致pos失效
        check_n_alloc(1);

        auto e_it = begin() + begin_num - 1;
        for (auto it = end() - 1; it != e_it; it --)
            *(it + 1) = *it;

        *(begin() + begin_num) = val;

        _Base::_m_add_finish_ptr();
        _Base::_m_add_str_end();
    }

    // 一次内存增大至少一倍
    void check_n_alloc(size_type n)
    {
        size_type left_space = _capacity() - _size();
        // 需要申请内存
        if (left_space < n)
        {
            if (n < _capacity())
                _Base::_m_reallocate();  // double
            else
                _Base::_m_reallocate(_capacity() + n);
        }
    }

    std::pair<T*, T*>
        alloc_n_copy(_CIt begin, _CIt end)
    {
        auto data = _Base::_m_allocate(end - begin);
        return {data, std::uninitialized_copy(begin, end, data)};  // 刚好用完
    }
};

template<typename T, typename Alloc>
typename BString<T, Alloc>::_It
BString<T, Alloc>::erase(_It pos)
{
    if (pos < begin() || pos >= end()) return pos;

    for (auto it = pos; it != end() - 1; it ++)
        *it = *(it + 1);

    _Base::_m_sub_finish_ptr();
    _Base::_m_add_str_end();

    return pos;
}

template<typename T, typename Alloc>
typename BString<T, Alloc>::_It
BString<T, Alloc>::erase(_It first, _It last)
{
    if (last < first) return _It();

    if (first < begin() || last > end()) return _It();

    size_type erase_num = last - first;

    auto it = first;
    for (auto num = erase_num; num > 0; num --)
        *it++ = *last++;

    _Base::_m_sub_finish_ptr(erase_num);
    _Base::_m_add_str_end();

    return first;
}

template<typename T, typename Alloc>
inline bool operator<(const BString<T, Alloc>& lhs, const BString<T, Alloc>& rhs)
{
    return lhs.compare(rhs) < 0;
}

template<typename T, typename Alloc>
inline bool operator>(const BString<T, Alloc>& lhs, const BString<T, Alloc>& rhs)
{
    return lhs.compare(rhs) > 0;
}

template<typename T, typename Alloc>
inline bool operator==(const BString<T, Alloc>& lhs, const BString<T, Alloc>& rhs)
{
    return lhs.compare(rhs) == 0;
}

template<typename T, typename Alloc>
inline bool operator!=(const BString<T, Alloc>& lhs, const BString<T, Alloc>& rhs)
{
    return lhs.compare(rhs) != 0;
}

template<typename T, typename Alloc>
std::ostream& operator<<(std::ostream& stream, const BString<T, Alloc>& __str)
{
    stream << __str.data();
    return stream;
}

using String = BString<char>;

template<typename _String, typename T = char>
_String _to_String(int(*_convfunc)(T*, std::size_t, const T*, va_list),
                    std::size_t _n, const T* _fmt, ...)
{
    T* array = new T(sizeof(T) * _n);

    va_list _args;
    va_start(_args, _fmt);
    const int len = _convfunc(array, _n, _fmt, _args);
    va_end(_args);

    _String val(array, len);
    delete[] array;

    return std::move(val);
}

inline String
to_String(int __val)
{
    return _to_String<String>(std::vsnprintf, 4 * sizeof(int), "%d", __val);
}

inline String
to_String(unsigned __val)
{
    return _to_String<String>(std::vsnprintf, 4 * sizeof(unsigned), "%u", __val);
}

inline String
to_String(long __val)
{
    return _to_String<String>(std::vsnprintf, 4 * sizeof(long), "%ld", __val);
}

inline String
to_String(unsigned long __val)
{
    return _to_String<String>(&std::vsnprintf,
                    4 * sizeof(unsigned long),
                    "%lu", __val); }

inline String
to_String(long long __val)
{
    return _to_String<String>(&std::vsnprintf,
                    4 * sizeof(long long),
                    "%lld", __val); }

inline String
to_String(unsigned long long __val)
{
    return _to_String<String>(&std::vsnprintf,
                    4 * sizeof(unsigned long long),
                    "%llu", __val); }

inline String
to_String(float __val)
{
    const int __n = 
        __gnu_cxx::__numeric_traits<float>::__max_exponent10 + 20;
    return _to_String<String>(&std::vsnprintf, __n,
                    "%f", __val);
}

inline String
to_String(double __val)
{
    const int __n = 
        __gnu_cxx::__numeric_traits<double>::__max_exponent10 + 20;
    return _to_String<String>(&std::vsnprintf, __n,
                        "%f", __val);
}

inline String
to_String(long double __val)
{
    const int __n = 
        __gnu_cxx::__numeric_traits<long double>::__max_exponent10 + 20;
    return _to_String<String>(&std::vsnprintf, __n,
                        "%Lf", __val);
}

}

#endif