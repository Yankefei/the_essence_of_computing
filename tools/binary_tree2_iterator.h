#ifndef _TOOLS_BINARY_TREE2_ITERATOR_HPP_
#define _TOOLS_BINARY_TREE2_ITERATOR_HPP_

#include <memory>
#include <cassert>

#include "stream.h"
#include "stack.h"
#include "queue.h"
#include "string.hpp"

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
    Tree<T>&  tree_;
    BtNodePtr ptr_;
    String    remark_;

public:
    TreeIterator(Tree<T>& bt): tree_(bt), ptr_(nullptr) {}
    virtual ~TreeIterator() {}

    T& operator*() { return ptr_->data_;}
    const T& operator*() const { return ptr_->data;}
    bool IsDone() const { return ptr_ == nullptr;}
    const String& Remark() { return remark_; };

    virtual void First() = 0;
    virtual void operator++() = 0;
};

template<typename T, template <typename T1> class Tree>
class NicePreIterator : public TreeIterator<T, Tree>
{
    typedef typename TreeIterator<T, Tree>::BtNodePtr BtNodePtr;
protected:
    Stack<BtNodePtr>          st_;

    using TreeIterator<T, Tree>::tree_;
    using TreeIterator<T, Tree>::ptr_;
    using TreeIterator<T, Tree>::remark_;

public:
    NicePreIterator(Tree<T>& bt)
        : TreeIterator<T, Tree>(bt)
        { remark_ = "NicePre"; }
    ~NicePreIterator() {}

    void First()
    {
        ptr_ = nullptr;
        if (tree_.get_root() != nullptr)
        {
            st_.push(tree_.get_root());
            operator++();
        }
    }

    // 想要代码不断写得好，就需要学习更好地代码~
    void operator++()
    {
        if(!st_.empty())
        {
            ptr_ = st_.top();
            st_.pop();

            if (ptr_->right_tree_)
            {
                st_.push(ptr_->right_tree_);
            }
            if (ptr_->left_tree_)
            {
                st_.push(ptr_->left_tree_);
            }
        }
        else
        {
            ptr_ = nullptr;
        }
    }
};

template<typename T, template <typename T1> class Tree>
class NiceInIterator : public TreeIterator<T, Tree>
{
    typedef typename TreeIterator<T, Tree>::BtNodePtr BtNodePtr;
protected:
    Stack<BtNodePtr>          st_;
    BtNodePtr                 p_;

    using TreeIterator<T, Tree>::tree_;
    using TreeIterator<T, Tree>::ptr_;
    using TreeIterator<T, Tree>::remark_;

public:
    NiceInIterator(Tree<T>& bt)
        : TreeIterator<T, Tree>(bt), p_(nullptr)
        { remark_ = "NiceIn"; }
    ~NiceInIterator() {}

    void First()
    {
        ptr_ = nullptr;
        p_ = tree_.get_root();
        if (p_ != nullptr)
        {
            operator++();
        }
    }

    void operator++()
    {
        if (p_ == nullptr && st_.empty())
        {
            ptr_ = nullptr;
            return;
        }

        if (!st_.empty() || p_)
        {
            while(p_)
            {
                st_.push(p_);
                p_ = p_->left_tree_;
            }
            p_ = st_.top();
            st_.pop();
            ptr_ = p_;

            p_ = p_->right_tree_;
        }
    }
};

template<typename T, template <typename T1> class Tree>
class NicePastIterator : public NiceInIterator<T, Tree>
{
    typedef typename TreeIterator<T, Tree>::BtNodePtr BtNodePtr;
protected:
    BtNodePtr      flower_;

    using TreeIterator<T, Tree>::tree_;
    using TreeIterator<T, Tree>::ptr_;
    using TreeIterator<T, Tree>::remark_;
    using NiceInIterator<T, Tree>::st_;
    using NiceInIterator<T, Tree>::p_;

public:
    NicePastIterator(Tree<T>& bt) : NiceInIterator<T, Tree>(bt),
        flower_(nullptr)
    {
        remark_ = "NicePast";
    }
    ~NicePastIterator() {}

    void First()
    {
        ptr_ = nullptr;
        p_ = tree_.get_root();
        if (p_ != nullptr)
        {
            operator++();
        }
    }

    void operator++()
    {
        if (p_ == nullptr  && st_.empty())
        {
            ptr_ = nullptr;
            return;
        }

        while (p_ || !st_.empty())
        {
            while (p_)
            {
                st_.push(p_);
                p_ = p_->left_tree_;
            }

            p_ = st_.top();
            st_.pop();

            if (p_->right_tree_ && flower_ != p_->right_tree_)
            {
                st_.push(p_);  // 重新入队
                p_ = p_->right_tree_;
            }
            else
            {
                ptr_ = flower_ = p_;
                p_ = nullptr;
                break;
            }
        }
    }
};


template<typename T, template <typename T1> class Tree>
class LevelIterator : public TreeIterator<T, Tree>
{
    typedef typename TreeIterator<T, Tree>::BtNodePtr BtNodePtr;
protected:
    NQueue<BtNodePtr>          queue_;
    BtNodePtr                  p_;

    using TreeIterator<T, Tree>::tree_;
    using TreeIterator<T, Tree>::ptr_;
    using TreeIterator<T, Tree>::remark_;

public:
    LevelIterator(Tree<T>& bt)
        : TreeIterator<T, Tree>(bt)
        { remark_ = "Level"; }
    ~LevelIterator() {}

    void First()
    {
        ptr_ = nullptr;
        p_ = tree_.get_root();
        if (p_ != nullptr)
        {
            queue_.push(p_);
            operator++();
        }
    }

    void operator++()
    {
        if(!queue_.empty())
        {
            p_ = queue_.front();
            queue_.pop_front();
            ptr_ = p_;

            if (p_->left_tree_)
                queue_.push(p_->left_tree_);
            if (p_->right_tree_)
                queue_.push(p_->right_tree_);
        }
        else
        {
            ptr_ = nullptr;
        }
    }
};


template<typename T, template <typename T1> class Tree>
struct StkNode
{
protected:
    typedef typename Tree<T>::Node*  BtNodePtr;
public:
    BtNodePtr                 pnode;
	int                       popnum;
public:
	StkNode(BtNodePtr p=NULL):pnode(p),popnum(0){}
};

template<typename T, template <typename T1> class Tree>
class StPastIterator : public TreeIterator<T, Tree>
{
    typedef typename TreeIterator<T, Tree>::BtNodePtr BtNodePtr;

    typedef StkNode<T, Tree>     StNode; // Tree 可不需要携带模板参数
protected:
    Stack<StNode>          st_;

    using TreeIterator<T, Tree>::tree_;
    using TreeIterator<T, Tree>::ptr_;
    using TreeIterator<T, Tree>::remark_;


public:
    StPastIterator(Tree<T>& bt)
        : TreeIterator<T, Tree>(bt)
        { remark_ = "StPast"; }
    ~StPastIterator() {}

    void First()
    {
        ptr_ = nullptr;
        st_.push(StNode(tree_.get_root()));
        operator++();
    }

    void operator++()
    {
        if (st_.empty())
        {
            ptr_ = nullptr;
            return;
        }

        while(!st_.empty())
        {
            StNode node = st_.top();
            st_.pop();
            if (++node.popnum == 3)
            {
                ptr_ = node.pnode;
                return;
            }
            else
            {
                st_.push(node);
                if (node.popnum == 1 && node.pnode->left_tree_ != nullptr)
                {
                    st_.push(StNode(node.pnode->left_tree_));
                }
                else if (node.popnum == 2 && node.pnode->right_tree_ != nullptr)
                {
                    st_.push(StNode(node.pnode->right_tree_));
                }
            }
        }
    }
};


template<typename T, template <typename T1> class Tree>
class StInIterator : public TreeIterator<T, Tree>
{
    typedef typename TreeIterator<T, Tree>::BtNodePtr BtNodePtr;

    typedef StkNode<T, Tree>     StNode; // Tree 可不需要携带模板参数
protected:
    Stack<StNode>          st_;

    using TreeIterator<T, Tree>::tree_;
    using TreeIterator<T, Tree>::ptr_;
    using TreeIterator<T, Tree>::remark_;


public:
    StInIterator(Tree<T>& bt)
        : TreeIterator<T, Tree>(bt)
        { remark_ = "StIn"; }
    ~StInIterator() {}

    void First()
    {
        ptr_ = nullptr;
        st_.push(StNode(tree_.get_root()));
        operator++();
    }

    void operator++()
    {
        if (st_.empty())
        {
            ptr_ = nullptr;
            return;
        }

        while(!st_.empty())
        {
            StNode node = st_.top();
            st_.pop();
            if (++node.popnum == 2)
            {
                if (node.pnode->right_tree_ != nullptr)
                {
                    st_.push(StNode(node.pnode->right_tree_));
                }
                ptr_ = node.pnode;
                break;
            }
            else
            {
                st_.push(node); // 重新入栈
                if (node.popnum == 1 && node.pnode->left_tree_ != nullptr)
                {
                    st_.push(StNode(node.pnode->left_tree_));
                }
            }
        }
    }
};

}

#endif