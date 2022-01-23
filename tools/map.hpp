#ifndef _TOOLS_MAP_H_
#define _TOOLS_MAP_H_

#include <memory>
#include <cassert>

// 必须含有parent_指针的node节点
#include "binary_rb_tree2.h"
#include "binary_avl_tree3.h"

#include "pair.hpp"

#include "iterator_base.h"

namespace tools
{

// Tree 的模板参数_Node, _Alloc 和 Node, Alloc在内部完全对应
template<typename Key,
         typename Val,
         typename Alloc,
         template <typename _T1> class Node,
         template <typename _T2,
                  typename _Alloc,
                  template <typename _T3> class _Node> class Tree>
class Map
{
    typedef Tree<Pair<Key, Val>, Alloc, Node>  TreeImpl;

public:

private:
    TreeImpl  _impl_tree;
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