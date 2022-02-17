#ifndef _TOOLS_SET_H_
#define _TOOLS_SET_H_

#include <memory>
#include <cassert>

// 内部的node节点必须含有parent_指针
#include "binary_rb_tree/binary_rb_tree2.h"
#include "binary_avl_tree/binary_avl_tree3.h"

#include "iterator_base.h"

namespace tools
{

template< typename Key,
          typename Alloc,
          template<typename T1> class _Node,
          template <typename T2,
                    typename _Alloc,
                    template <typename T3> class _Node1>  class Tree>
class Set
{
    typedef Tree<Key, Alloc, _Node>  TreeImpl;
    typedef Key                      value_type;
    typedef std::size_t              size_type;

    typedef typename TreeImpl::Node          Node;
    typedef typename TreeImpl::RemoveRes  RemoveRes;
    typedef typename TreeImpl::InsertRes  InsertRes;

    struct Iter : public  _Forwardit<value_type, ptrdiff_t>
    {
        Iter() = default;
        Iter(Node* node) : node_(node) {}

        Iter& operator=(const Iter& rhs)
        {
            node_ = rhs.node_;
            return *this;
        }

        value_type* operator->()
        {
            return &**this;
        }

        value_type& operator*()
        {
            return node_->data_;
        }

        Iter& operator++()
        {
            node_ = TreeImpl::next(node_);
            return *this;
        }

        Iter operator++(int)
        {
            Iter temp = *this;
            ++*this;
            return temp;
        }

        Iter& operator--()
        {
            node_ = TreeImpl::prev(node_);
            return *this;
        }

        Iter operator--(int)
        {
            Iter temp = *this;
            --*this;
            return temp;
        }

        bool operator==(const Iter& rhs)
        {
            return node_ == rhs.node_;
        }

        bool operator!=(const Iter& rhs)
        {
            return !(*this == rhs);
        }

    private:
        Node* node_{nullptr};
    };

    struct CIter : public  _Forwardit<value_type, ptrdiff_t>
    {
        CIter() = default;
        CIter(Node* node) : node_(node) {}

        const value_type* operator->()   const
        {
            return &**this;
        }

        const value_type& operator*()  const
        {
            return node_->data_;
        }

        CIter& operator++()
        {
            node_ = TreeImpl::next(node_);
            return *this;
        }

        CIter operator++(int)
        {
            CIter temp = *this;
            ++*this;
            return temp;
        }

        CIter& operator--()
        {
            node_ = TreeImpl::prev(node_);
            return *this;
        }

        CIter operator--(int)
        {
            CIter temp = *this;
            --*this;
            return temp;
        }

        bool operator==(const CIter& rhs) const
        {
            return node_ == rhs.node_;
        }

        bool operator!=(const CIter& rhs) const
        {
            return !(*this == rhs);
        }

    private:
        Node* node_{nullptr};
    };

public:
    Set() : impl_tree_() {}
    ~Set() {}

    Set(std::initializer_list<value_type> list)
    {
        for(auto p = list.begin(); p != list.end(); p ++)
            insert(*p);
    }

    Set(const Set& rhs) : impl_tree_(rhs.impl_tree_), ele_size_(rhs.ele_size_)
    {}

    Set& operator=(const Set& rhs)
    {
        if(this != &rhs)
        {
            impl_tree_ = rhs.impl_tree_;
            ele_size_ = rhs.ele_size_;
        }
        return *this;
    }

    Set(Set&& rhs) noexcept
    {
        impl_tree_ = std::forward<TreeImpl>(rhs.impl_tree_);
        ele_size_ = rhs.ele_size_;
        rhs.ele_size_ = 0;
    }

    Set& operator=(Set&& rhs) noexcept
    {
        if (this != &rhs)
        {
            impl_tree_ = std::forward<TreeImpl>(rhs.impl_tree_);
            ele_size_ = rhs.ele_size_;
            rhs.ele_size_ = 0;
        }
        return *this;
    }

    InsertRes insert(const value_type& val)
    {
        InsertRes res = impl_tree_.c_insert(val);
        if (res.second) ele_size_ ++;
        return res;
    }

    size_type erase(const Key& key)
    {
        RemoveRes res = impl_tree_.c_remove(key);
        size_type n = res.second ? 1 : 0;
        ele_size_ -= n;
        return n;
    }

    Iter erase(Iter It)
    {
        RemoveRes res = impl_tree_.c_remove(*It);
        if (res.second)
            ele_size_--;
        return Iter(res.first);
    }

    Iter find(const Key& key)
    {
        return Iter(impl_tree_.find(key));
    }

    CIter find(const Key& key) const
    {
        return CIter(impl_tree_.find(key));
    }

    Iter begin()
    {
        return Iter(impl_tree_.begin());
    }

    CIter cbegin() const
    {
        return CIter(impl_tree_.begin());
    }

    Iter end()
    {
        return Iter();
    }

    CIter cend() const
    {
        return CIter();
    }

    size_t size() const
    {
        return ele_size_;
    }

    bool empty() const
    {
        return size() == 0;
    }

    void clear()
    {
        impl_tree_.clear();
        ele_size_ = 0;
    }

private:
    TreeImpl  impl_tree_;
    size_type ele_size_{0};
};

// rb_tree set
template<typename Key,
        typename Alloc,
        template<typename T1> class Node = rb_tree_2::_RbNode,
        template <typename T2,
                 typename _Alloc,
                 template <typename T3> class RbNode> class Tree = rb_tree_2::RbTree>
using RSet_Half = Set<Key, Alloc, Node, Tree>;

template<typename Key,
        typename Alloc = std::allocator<Key>>
using RSet = RSet_Half<Key, Alloc>;

// avl_tree set
template<typename Key,
        typename Alloc,
        template<typename T1> class Node = avl_tree_3::_BNode,
        template< typename T2,
                typename _Alloc,
                template <typename T3> class BNode> class Tree = avl_tree_3::AvlTree>
using ASet_Half = Set<Key, Alloc, Node, Tree>;

template<typename Key,
        typename Alloc = std::allocator<Key>>
using ASet = ASet_Half<Key, Alloc>;

}

#endif