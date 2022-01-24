#ifndef _TOOLS_BALANCE_TREE_H_
#define _TOOLS_BALANCE_TREE_H_

#include <memory>
#include <cassert>

#include "balance_tree_base.h"

#include "algorithm.hpp"

namespace tools
{

namespace b_tree
{

template<typename T>
struct _BNode;

template<typename T>
struct Entry
{
    T data_;
    union
    {
        _BNode<T>*  next_;
        // Item  item;    仅定义到索引层, 不定义具体的数据，叶子节点的next_指针设置为空
    };
};

template<typename T>
struct _BNode
{
    _BNode() = default;
    int      m_{0};          // 数组包含的元素个数
    Entry<T> array_[0];      // 数据内存后置
};

/*B树*/
template<typename T,
        typename Alloc = std::allocator<T>,
        template <typename T1> class BNode = _BNode>
class BalanceTree : protected BalanceTree_Base<T, BNode, Alloc, sizeof(Entry<T>)>
{
    typedef BalanceTree_Base<T, BNode, Alloc, sizeof(Entry<T>)> BalanceTreeBase;
    typedef typename BalanceTreeBase::Node Node;
    typedef Node*  Root;
public:
    using BalanceTreeBase::buy_node;
    using BalanceTreeBase::free_node;
    using BalanceTreeBase::_m_impl;

public:
    BalanceTree(size_t m) : BalanceTreeBase(m)
    {
        _m_impl._root = buy_node();
    }

    ~BalanceTree()
    {
        free_node(_m_impl._root);
    }

    Node* get_root()
    {
        return _m_impl._root;
    }

private:


};

}
}

#endif