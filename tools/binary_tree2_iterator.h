#ifndef _TOOLS_BINARY_TREE2_ITERATOR_HPP_
#define _TOOLS_BINARY_TREE2_ITERATOR_HPP_

#include <memory>
#include <cassert>

#include "stream.h"
#include "stack.h"
#include "queue.h"

#include "binary_tree.h"

namespace tools
{

// 模板的模板
template<typename T, template <typename T1> class Tree>
class TreeIterator
{
protected:
    typedef typename Tree<T>::Node*  BtNodePtr;
protected:
    Tree<T>&  _tree;
    BtNodePtr _ptr;

public:
    TreeIterator(Tree<T>& bt): _tree(bt), _ptr(nullptr) {}
    virtual ~TreeIterator() {}
};

template<typename T, template <typename T1> class Tree>
class NicePreIterator : public TreeIterator<T, Tree>
{
    typedef typename TreeIterator<T, Tree>::BtNodePtr BtNodePtr;
protected:
    Stack<_Node<T>*>          st_;
    BtNodePtr                 p_;

public:
    NicePreIterator(Tree<T>& bt)
        : TreeIterator<T, Tree>(bt), p_(nullptr) {}
    ~NicePreIterator() {}
};

template<typename T, template <typename T1> class Tree>
class NiceInIterator : public TreeIterator<T, Tree>
{
    typedef typename TreeIterator<T, Tree>::BtNodePtr BtNodePtr;
protected:
    Stack<_Node<T>*>          st_;
    BtNodePtr                 p_;

public:
    NiceInIterator(Tree<T>& bt)
        : TreeIterator<T, Tree>(bt), p_(nullptr) {}
    ~NiceInIterator() {}

    void First()
    {

    }
};

template<typename T, template <typename T1> class Tree>
class NicePastIterator : public NiceInIterator<T, Tree>
{
    typedef typename TreeIterator<T, Tree>::BtNodePtr BtNodePtr;
protected:
    BtNodePtr      flower_;

public:
    NicePastIterator(Tree<T>& bt) : NiceInIterator<T, Tree>(bt), flower_(nullptr) {}
    ~NicePastIterator() {}
};


}

#endif