#ifndef _TOOLS_LIST_HPP_
#define _TOOLS_LIST_HPP_

#include <memory>
#include <cassert>

#include "stream.h"
#include "algorithm.hpp"
#include "iterator_base.h"

namespace tools
{

template<typename T>
struct _Entry
{
    _Entry(const T& val) : data_(val) {}
    _Entry(T&& val) : data_(std::forward<T>(val)) {}
    _Entry* next_{nullptr};
    _Entry* prev_{nullptr};
    T       data_;
};

template<typename T, template <typename T1> class LEntry,
         typename Alloc>
class List_Base
{
public:
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<LEntry<T>> _LEntry_alloc_type;

    typedef typename std::allocator_traits<_LEntry_alloc_type>
        rebind_traits;

    typedef typename rebind_traits::pointer  pointer;

    typedef LEntry<T>        Entry;

public:
    struct EntryBase_Impl : public _LEntry_alloc_type {};  // 仅仅用来指导申请内存
    static EntryBase_Impl _m_entry_impl_;                  // 无状态，设置为静态变量

    List_Base() {}
    ~List_Base() {}

public:
    // 一个重载形式就够用
    template<typename... Args>
    Entry* buy_entry(Args&&... args)
    {
        pointer p = rebind_traits::allocate(_m_entry_impl_, 1);
        rebind_traits::construct(_m_entry_impl_, p, std::forward<Args>(args)...);
        return static_cast<Entry*>(p);
    }

    // Entry* buy_entry(const T& val)
    // {
    //     pointer p = rebind_traits::allocate(_m_entry_impl_, 1);
    //     rebind_traits::construct(_m_entry_impl_, p, val);
    //     return static_cast<Entry*>(p);
    // }

    // Entry* buy_entry(T&& val)
    // {
    //     pointer p = rebind_traits::allocate(_m_entry_impl_, 1);
    //     rebind_traits::construct(_m_entry_impl_, p, std::forward<T>(val));
    //     return static_cast<Entry*>(p);
    // }

    void free_entry(Entry* ptr)
    {
        rebind_traits::destroy(_m_entry_impl_, ptr);
        rebind_traits::deallocate(_m_entry_impl_, ptr, 1);
    }
};

template <typename T,
         template <typename T1> class LEntry,
         typename Alloc>
typename List_Base<T, LEntry, Alloc>::EntryBase_Impl
    List_Base<T, LEntry, Alloc>::_m_entry_impl_;

template <typename T,
          template <typename T1> typename LEntry = _Entry,
          typename Alloc = std::allocator<T>>
class List : protected List_Base<T, LEntry, Alloc>
{
public:
    typedef List_Base<T, LEntry, Alloc>   ListBase;
    typedef typename ListBase::Entry      Entry;
    typedef T                             value_type;
    typedef size_t                        size_type;

    struct Iter : public  _Bidit<value_type, ptrdiff_t>
    {
        friend class List;
        Iter(Entry* ptr) : ptr_(ptr) {}
        Iter(const Iter& val) { ptr_ = val.ptr_; }
        Iter& operator=(const Iter& val)
        {
            if (this == &val) return *this;
            ptr_ = val.ptr_;
            return *this;
        }
        value_type* operator->()
        {
            return &**this;
        }
        value_type& operator*()
        {
            return ptr_->data_;
        }
        Iter operator++(int)
        {
            Iter t = *this;
            ++*this;
            return t;
        }
        Iter& operator++()
        {
            ptr_ = ptr_->next_;
            return *this;
        }
        Iter operator--(int)
        {
            Iter t = *this;
            --*this;
            return t;
        }
        Iter& operator--()
        {
            ptr_ = ptr_->prev_;
            return *this;
        }
        bool operator==(const Iter& val) const
        {   return ptr_ == val.ptr_; }
        bool operator!=(const Iter& val) const
        {   return !(*this == val); }
    private:
        Entry* ptr_{nullptr};
    };

    struct CIter : public _Bidit<value_type, ptrdiff_t>
    {
        friend class List;
        CIter(Entry* ptr) : ptr_(ptr) {}
        CIter(const CIter& val) { ptr_ = val.ptr_; }
        CIter& operator=(const CIter& val)
        {
            if (this == &val) return *this;
            ptr_ = val.ptr_;
            return *this;
        }
        const value_type* operator->()
        {
            return &**this;
        }
        const value_type& operator*()
        {
            return ptr_->data_;
        }
        CIter operator++(int)
        {
            CIter t = *this;
            ++*this;
            return t;
        }
        CIter& operator++()
        {
            ptr_ = ptr_->next_;
            return *this;
        }
        CIter operator--(int)
        {
            CIter t = *this;
            --*this;
            return t;
        }
        CIter& operator--()
        {
            ptr_ = ptr_->prev_;
            return *this;
        }
        bool operator==(const CIter& val) const
        {   return ptr_ == val.ptr_; }
        bool operator!=(const CIter& val) const
        {   return !(*this == val); }
    private:
        Entry* ptr_{nullptr};
    };

public:
    using ListBase::buy_entry;
    using ListBase::free_entry;

public:
    List() { init_header(); }
    ~List()
    {
        destory();
        assert(header_ != nullptr);
        assert(header_->next_ == header_);
        assert(header_->prev_ == header_);
        free_entry(header_);
    }

    List(std::initializer_list<value_type> list)
    {
        init_header();
        for(auto p = list.begin(); p != list.end(); p ++)
            push_back(*p);
    }

    List(const List& rhs)
    {
        init_header();
        copy(rhs);
    }
    List& operator=(const List& rhs)
    {
        if (this != &rhs)
        {
            clear();
            copy(rhs);
        }
        return *this;
    }
    List(List&& rhs) noexcept
    {
        init_header();
        std::swap(header_, rhs.header_);
        std::swap(ele_size_, rhs.ele_size_);
    }
    List& operator=(List&& rhs)  noexcept
    {
        if (this != &rhs)
        {
            clear();
            std::swap(header_, rhs.header_);
            std::swap(ele_size_, rhs.ele_size_);
        }
        return *this;
    }

    value_type& front()
    {
        return *begin();
    }

    value_type& back()
    {
        Iter t = end();
        t--;
        return *t;
    }

    // 在pos前插入 val
    Iter insert(CIter pos, const T& val)
    {
        Entry* new_node = buy_entry(val);
        _push_front(new_node, pos.ptr_);
        return Iter(new_node);
    }

    template<typename... Args>
    Iter emplace(CIter pos, Args&&... args)
    {
        Entry* new_node = buy_entry(std::forward<Args>(args)...);
        _push_front(new_node, pos.ptr_);
        return Iter(new_node);
    }

    void push_back(const T& val)
    {
        Entry* new_node = buy_entry(val);
        _push_front(new_node, header_);
    }

    void push_back(T&& val)
    {
        Entry* new_node = buy_entry(std::forward<T>(val));
        _push_front(new_node, header_);
    }

    template<typename ... Args>
    void emplace_back(Args&&... args)
    {
        Entry* new_node = buy_entry(std::forward<Args>(args)...);
        _push_front(new_node, header_);
    }

    void pop_back()
    {
        if (empty()) return;
        _pop_front(header_);
    }

    void push_front(const T& val)
    {
        Entry* new_node = buy_entry(val);
        _push_back(new_node, header_);
    }

    void push_front(T&& val)
    {
        Entry* new_node = buy_entry(std::forward<T>(val));
        _push_back(new_node, header_);
    }

    template<typename ... Args>
    void emplace_front(Args&&... args)
    {
        Entry* new_node = buy_entry(std::forward<Args>(args)...);
        _push_back(new_node, header_);
    }

    void pop_front()
    {
        if (empty()) return;
        _pop_back(header_);
    }

    Iter erase(Iter pos)
    {
        if (pos == end()) return pos;
        Iter re = ++pos;
        _pop_front(re.ptr_);
        return re;
    }

    bool empty() {  return size() == 0; }

    size_type size() { return ele_size_; }

    void clear()
    {
        destory();
    }

    Iter begin()   const
    {
        return Iter(header_->next_);
    }
    Iter end()    const
    {
        return Iter(header_);
    }

    CIter  cbegin()  const
    {
        return CIter(header_->next_);
    }
    CIter  cend()  const
    {
        return CIter(header_);
    }

public:
    // 排序 从小到大 插入排序
    void sort()
    {
        Entry* new_ptr = header_->next_;
        Entry* tmp = nullptr;
        while(new_ptr != header_)
        {
            // 分离出new_ptr节点
            new_ptr->next_->prev_ = new_ptr->prev_;
            new_ptr->prev_->next_ = new_ptr->next_;

            // for (Entry* ptr = header_->next_; ptr != header_; ptr = ptr->next_)
            // {
            //     if (ptr == new_ptr->prev_)
            //         stream << ptr->data_ << " ] ";
            //     else
            //         stream << ptr->data_ << "   ";
            // }
            // stream << ", insert : "<< new_ptr->data_ <<std::endl;

            tmp = new_ptr->next_;
            insert(new_ptr, header_->next_, new_ptr->prev_);
            ele_size_ --; // 维护ele_size_
            new_ptr = tmp;
        }
    }

    // 逆序, 遍历一遍, 同时修改next_和prev_指针
    void reverse()
    {
        Entry* last_ptr = header_;
        Entry* ptr = last_ptr->next_;
        Entry* tmp = nullptr;
        while(ptr != header_)
        {
            last_ptr->prev_ = ptr;
            tmp = ptr->next_;
            ptr->next_ = last_ptr;

            last_ptr = ptr;
            ptr = tmp;
        }
        // 再把最后header部分完善
        ptr->next_ = last_ptr;
        last_ptr->prev_ = ptr;
    }

    // 移除元素 val
    void remove(const T& val)
    {
        Iter tag = find(val);
        if (tag == end()) return;

        erase(tag);
    }

    // 转移元素it到pos前的位置
    void splice(const Iter& pos, List& rhs, Iter it)
    {
        Entry* it_ptr = it.ptr_;
        Entry* it_last_node = it_ptr->prev_;
        it_last_node->next_ = it_ptr->next_;
        it_ptr->next_->prev_ = it_last_node;
        rhs.ele_size_ --;
        // it_ptr 调用_push_front后指针会被修改，所以需要放在最后
        _push_front(it_ptr, pos.ptr_);
    }

    // 归并两个已经排序的链表，合并后rhs为空
    // 如果merge前没有将List调用sort的话，合并后的链表也是无序的
    void merge(List& rhs)
    {
        Entry* rhs_ptr = rhs.header_->next_;
        Entry* ptr = header_->next_;
        Entry* tmp = nullptr;
        while(rhs_ptr != rhs.header_ && ptr != header_)
        {
            if (alg::le(rhs_ptr->data_, ptr->data_))
            {
                tmp = rhs_ptr->next_;
                _push_front(rhs_ptr, ptr);
                rhs_ptr = tmp;
            }
            else
            {
                ptr = ptr->next_;
            }
        }

        while (rhs_ptr != rhs.header_)
        {
            tmp = rhs_ptr->next_;
            _push_front(rhs_ptr, header_);
            rhs_ptr = tmp;
        }

        // 清理rhs
        rhs.header_->next_ = rhs.header_->prev_ = rhs.header_;
        rhs.ele_size_ = 0;        
    }

private:
    void copy(const List& rhs)
    {
        for (auto it = rhs.cbegin(); it != rhs.cend(); ++it)
        {
            push_back(*it);
        }
    }

    // 将node按照递增顺序插入到[begin, end]之间的区域
    void insert(Entry* node, Entry* begin, Entry* end)
    {
        Entry* real_end = end->next_;
        for (; begin != real_end; begin = begin->next_)
        {
            if (alg::le(node->data_, begin->data_))
            {
                _push_front(node, begin);
                return;
            }
        }
        _push_front(node, begin); // 最后将插入到end之后, real_end之前
    }

    // 插入在tag_pos前
    void _push_front(Entry* node, Entry* tag_pos)
    {
        Entry* last = tag_pos->prev_;
        node->next_ = tag_pos;
        node->prev_ = last;
        last->next_ = node;
        tag_pos->prev_ = node;
        ele_size_ ++;
    }
    // 插入在tag_pos后
    void _push_back(Entry* node, Entry* tag_pos)
    {
        Entry* next_node = tag_pos->next_;
        node->next_ = next_node;
        node->prev_ = tag_pos;
        next_node->prev_ = node;
        tag_pos->next_ = node;
        ele_size_ ++;
    }
    // 删除tag_pos前一个节点
    void _pop_front(Entry* tag_pos)
    {
        Entry* last = tag_pos->prev_;
        last->prev_->next_ = tag_pos;
        tag_pos->prev_ = last->prev_;
        free_entry(last);
        ele_size_ --;
    }
    // 删除tag_pos下一个节点
    void _pop_back(Entry* tag_pos)
    {
        Entry* next_node = tag_pos->next_;
        next_node->next_->prev_ = tag_pos;
        tag_pos->next_ = next_node->next_;
        free_entry(next_node);
        ele_size_ --;
    }

    Iter find(const T& val)
    {
        auto it = begin();
        for (; it != end(); it++)
        {
            if (alg::eq(*it, val))
                break;
        }
        return it;
    }

    void init_header()
    {
        header_ = buy_entry(T());
        header_->next_ = header_->prev_ = header_;
    }

    void destory()
    {
        for (Iter it = begin(); it != end();)
            it = erase(it);
        ele_size_ = 0;
    }

    Entry*      header_{nullptr};       // 头节点
    size_type   ele_size_{0};
};

template<typename T, template<typename T1> class Entry, typename Alloc>
std::ostream& operator<<(std::ostream& os, const List<T, Entry, Alloc>& rhs)
{
    for (const auto& e : rhs)
        os << e << " ";
    return os;
}

}

#endif