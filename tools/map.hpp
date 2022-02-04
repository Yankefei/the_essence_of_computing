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
    
    // TODO, 使用迭代器替换
    typedef typename TreeImpl::Node*                   Iter;
    typedef typename TreeImpl::Node const*             CIter;

public:
    Map() : impl_tree_() {};
    ~Map() {}

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
        impl_tree_ = std::move(rhs.impl_tree_);
        ele_size_ = rhs.ele_size_;
    }

    Map& operator=(Map&& rhs) noexcept
    {
        if (this != &rhs)
        {
            impl_tree_ = std::move(rhs.impl_tree_);
            ele_size_ = rhs.ele_size_;
        }
        return *this;
    }

    bool insert(const value_type& val)
    {
        bool res = impl_tree_.insert(val);
        if (res) ele_size_ ++;
        return res;
    }

    size_type erase(const Key& key)
    {
        bool res = impl_tree_.remove(MakePair<Key, Val>(key, Val()));
        size_type s = res ? 1 : 0;
        return s;
    }

    Iter erase(CIter It)
    {
        // todo
        Iter next = impl_tree_.next(It);
    }

    Val& operator[](const Key& key)
    {

    }

    Iter find(const Key& key)
    {

    }

    CIter find(const Key& key) const
    {

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