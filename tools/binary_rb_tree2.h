#ifndef _TOOLS_BINARY_RB_TREE2_H_
#define _TOOLS_BINARY_RB_TREE2_H_

#include <memory>
#include <cassert>

#include "vector.hpp"
#include "queue.h"
#include "algorithm.hpp"

#include "binary_rb_tree_base.h"

#define HAS_COLOR
#include "binary_tree_print_util.h"

namespace tools
{

namespace rb_tree_2
{

template<typename T>
struct _RbNode
{
    _RbNode() = default;
    _RbNode(const T&val) : data_(val) {}

    _RbNode* left_tree_{nullptr};
    _RbNode* right_tree_{nullptr};
    _RbNode* parent_{nullptr};
    Color color_{Color::Black};
    T data_;
};

/*红黑树 非递归*/
template<typename T, template <typename T1> class RbNode = _RbNode, typename Alloc = std::allocator<T>>
class RbTree : protected RbTree_Base<T, _RbNode, Alloc>
{
    typedef RbTree_Base<T, RbNode, Alloc> RbTreeBase;
    typedef typename RbTreeBase::Node Node;
    typedef Node*  Root;

public:
    using RbTreeBase::buy_node;
    using RbTreeBase::free_node;
    using RbTreeBase::_m_impl;


public:
    RbTree() {}
    ~RbTree() { destory(_m_impl._root); }

    bool insert(const T& val)
    {

    }

    bool remove(const T& val)
    {

    }

    void in_order()
    {

    }

    void print_tree()
    {
        
    }

private:
    void destory(Node* ptr)
    {
        if (ptr)
        {
            destory(ptr->left_tree_);
            destory(ptr->right_tree_);
            free_node(ptr);
        }
    }


};

}
}

#endif