#ifndef _TOOLS_BALANCE_PLUS_TREE_H_
#define _TOOLS_BALANCE_PLUS_TREE_H_

#include <memory>
#include <cassert>

#include "algorithm.hpp"

#include "balance_tree_base.h"

namespace tools
{
namespace b_plus_tree
{

template<typename T>
struct _BNode;

template<typename T>
struct _Entry
{
    _Entry(const T& val) : data_(val) {}
    T data_;
    union
    {
        _BNode<T>*  next_{nullptr};
        // Item  item;    仅定义到索引层, 不定义具体的数据，叶子节点的next_指针设置为空
    };
};

template<typename T>
struct _BNode
{
    _BNode() = default;
    _BNode*      next_node_{nullptr};  // 指向下一个BNode的叶子节点(只有在叶子节点是才生效)
    int          size_{0};                // 数组包含的元素个数
    _Entry<T>**  array_{nullptr};            // 数据内存后置
};


/*B+树*/
template<typename T,
        typename Alloc = std::allocator<T>,
        template <typename T1> class BNode = _BNode,
        template <typename T1> class Entry = _Entry>
class BalancePlusTree : protected BalanceTree_Base<T, Alloc, BNode, Entry>
{
    typedef BalanceTree_Base<T, Alloc, BNode, Entry> BalanceTreeBase;
    typedef typename BalanceTreeBase::Node Node;
    typedef Node*  Root;

public:
    using BalanceTreeBase::buy_node;
    using BalanceTreeBase::free_node;
    using BalanceTreeBase::_m_impl;
    using BalanceTreeBase::buy_entry;
    using BalanceTreeBase::free_entry;

public:
    BalancePlusTree(size_t m/*B+树的阶*/) : BalanceTreeBase(m)
    {
        _m_impl._root = buy_node();
    }

    ~BalancePlusTree()
    {
        destory(_m_impl._root);
    }

    Node* get_root()
    {
        return _m_impl._root;
    }

private:
    void destory(Node* ptr)
    {
        if(ptr == nullptr || ptr->size_ == 0) return;

        for(int i = 0; i < ptr->size_; i++)
        {
            destory(ptr->array_[i]->next_);
        }
        free_node(ptr);
    }

private:

};

}
}

#endif