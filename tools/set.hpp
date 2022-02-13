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

private:
    TreeImpl  impl_tree_;
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