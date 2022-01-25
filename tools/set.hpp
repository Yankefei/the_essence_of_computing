#ifndef _TOOLS_SET_H_
#define _TOOLS_SET_H_

#include <memory>
#include <cassert>

// 内部的node节点必须含有parent_指针
#include "binary_rb_tree2.h"
#include "binary_avl_tree3.h"

#include "iterator_base.h"

namespace tools
{

template< typename Key,
          typename Alloc,
          template<typename T1> class Node,
          template <typename T2,
                    typename _Alloc,
                    template <typename T3> class _Node>  class Tree>
class Set
{
    typedef Tree<Key, Alloc, Node>  TreeImpl;
public:

private:
    TreeImpl  _impl_tree;
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