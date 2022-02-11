#ifndef _TOOLS_MAP_H_
#define _TOOLS_MAP_H_

#include <memory>
#include <cassert>

// 内部的node节点必须含有parent_指针
#include "binary_rb_tree/binary_rb_tree2.h"
#include "binary_avl_tree/binary_avl_tree3.h"

#include "pair.hpp"

#include "iterator_base.h"

namespace tools
{

// Tree 的模板参数_Node, _Alloc 和 Node, Alloc在内部完全对应
template<typename Key,
         typename Val,
         typename Alloc,
         template <typename _T1> class _Node,
         template <typename _T2,
                  typename _Alloc,
                  template <typename _T3> class _Node1> class Tree>
class Map
{
    typedef Tree<Pair<Key, Val>, Alloc, _Node>  TreeImpl;
    typedef Pair<Key, Val>          value_type;
    typedef std::size_t             size_type;

    typedef typename TreeImpl::Node          Node;
    typedef typename TreeImpl::RemoveRes  RemoveRes;
    typedef typename TreeImpl::InsertRes  InsertRes;

    struct Iter
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

    struct CIter
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
    Map() : impl_tree_() {};
    ~Map() {}

    Map(std::initializer_list<value_type> list)
    {
        for(auto p = list.begin(); p != list.end(); p ++)
            insert(*p);
    }

    Map(const Map& rhs) : impl_tree_(rhs.impl_tree_), ele_size_(rhs.ele_size_)
    {}

    Map& operator=(const Map& rhs)
    {
        if(this != &rhs)
        {
            impl_tree_ = rhs.impl_tree_;
            ele_size_ = rhs.ele_size_;
        }
        return *this;
    }

    Map(Map&& rhs) noexcept
    {
        impl_tree_ = std::forward<TreeImpl>(rhs.impl_tree_);
        ele_size_ = rhs.ele_size_;
        rhs.ele_size_ = 0;
    }

    Map& operator=(Map&& rhs) noexcept
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
        value_type t = MakePair<Key, Val>(key, Val());
        RemoveRes res = impl_tree_.c_remove(t);
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

    Val& operator[](const Key& key)
    {
        // 如果key不存在，则插入
        InsertRes res = impl_tree_.c_insert(MakePair<Key, Val>(key, Val()));
        if (res.second)
            ele_size_ ++;
        return res.first->data_.second;
    }

    Iter find(const Key& key)
    {
        return Iter(impl_tree_.find(MakePair<Key, Val>(key, Val())));
    }

    CIter find(const Key& key) const
    {
        return CIter(impl_tree_.find(MakePair<Key, Val>(key, Val())));
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

// rb_tree map
template<typename Key,
         typename Val,
         typename Alloc,
         template <typename _T1> class Node = rb_tree_2::_RbNode,
         template <typename T1,
                  typename _Alloc,
                  template <typename T2> class RbNode> class Tree = rb_tree_2::RbTree>
using RMap_Half = Map<Key, Val, Alloc, Node, Tree>;

template<typename Key,
         typename Val,
         typename Alloc = std::allocator<Pair<Key, Val>>>
using RMap = RMap_Half<Key, Val, Alloc>;

// avl_tree map
template <typename Key,
         typename Val,
         typename Alloc,
         template <typename _T1> class Node = avl_tree_3::_BNode,
         template< typename T1,
                typename _Alloc,
                template <typename T2> class BNode> class Tree = avl_tree_3::AvlTree>
using AMap_Half = Map<Key, Val, Alloc, Node, Tree>;

template<typename Key,
         typename Val,
         typename Alloc = std::allocator<Pair<Key, Val>>>
using AMap = AMap_Half<Key, Val, Alloc>;
}

#endif