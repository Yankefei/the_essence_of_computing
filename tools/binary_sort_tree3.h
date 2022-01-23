#ifndef _TOOLS_BINARY_SORT_TREE3_H_
#define _TOOLS_BINARY_SORT_TREE3_H_

#include <memory>
#include <cassert>

#include "binary_sort_tree_base.h"
#include "algorithm.hpp"

namespace tools
{

template<typename T>
struct _SNode3
{
    _SNode3() = default;
    _SNode3(const T&val) : data_(val) {}

    _SNode3* left_tree_{nullptr};
    _SNode3* right_tree_{nullptr};
    _SNode3* parent_{nullptr};
    T data_;
};

/*排序二叉树*/
template<typename T, template <typename T1> class SNode = _SNode3, typename Alloc = std::allocator<T>>
class BsTree : protected BsTree_Base<T, SNode, Alloc>
{
    typedef BsTree_Base<T, SNode, Alloc> BsTreeBase;
    typedef typename BsTreeBase::Node Node;
    typedef Node*  Root;

public:
    using BsTreeBase::buy_node;
    using BsTreeBase::free_node;
    using BsTreeBase::_m_impl;

public:
    BsTree() {}
    ~BsTree() { destory(_m_impl._root); }

    // 有一种处理重复数据的方案，就是在数据域增加一个频率的字段
    // 不过在关键字仅仅为数据的一部分这种情况，还必须使用其他的处理方案
    bool insert(const T& val)
    {
        return insert(&_m_impl._root, val);
    }

    // 递归版本
    bool insert2(const T& val)
    {
        return insert(_m_impl._root, val, nullptr);
    }

    bool remove(const T& val)
    {
        return remove(&_m_impl._root, val);
    }

    void InOrder() const
    {
        _InOrder(_m_impl._root);
        stream << std::endl;
    }

    void NiceInOrder() const
    {
        auto ptr = first();
        for (; ptr != nullptr; ptr = next(ptr))
        {
            stream << ptr->data_ << " ";
        }
        stream << std::endl;
    }

    void ResNiceInOrder() const
    {
        auto ptr = last();
        for (; ptr != nullptr; ptr = prev(ptr))
        {
            stream << ptr->data_ << " ";
        }
        stream << std::endl;
    }

    Node* find(const T& val) const
    {
        return find(_m_impl._root, val);
    }

private:
    bool insert(Node*& ptr, const T& val, Node* pptr)
    {
        bool res = false;
        if (ptr == nullptr)
        {
            ptr = buy_node(val);
            ptr->parent_ = pptr;
            res = true;
        }
        else if (alg::le(val, ptr->data_))
        {
            return insert(ptr->left_tree_, val, ptr);
        }
        else if (alg::le(ptr->data_, val))
        {
            return insert(ptr->right_tree_, val, ptr);
        }

        return res;
    }

    bool insert(Node** pptr, const T& val)
    {
        if (pptr == nullptr) return false;
        Node* pa = nullptr;
        Node* p = *pptr;
        while(p != nullptr && alg::neq(p->data_, val))
        {
            pa = p;
            p = alg::le(val, p->data_) ? p->left_tree_ : p->right_tree_;
        }
        if (p != nullptr && alg::eq(p->data_, val)) return false;

        // 在不需要区分左右的时候，先不区分，这样后面代码会好写很多，简洁很多
        // 尤其在下面remove的时候，最为明显
        p = buy_node(val);
        p->parent_ = pa;
        if (pa == nullptr)
            *pptr = p;
        else
        {
            if (alg::le(p->data_, pa->data_))
            {
                pa->left_tree_ = p;
            }
            else
            {
                pa->right_tree_ = p;
            }
        }

        return true;
    }

    // 删除的过程需要维护 parent_ 指针
    bool remove(Node** pptr, const T& val)
    {
        if (pptr == nullptr || *pptr == nullptr) return false;

        Node* ptr = find(*pptr, val);
        if (ptr == nullptr) return false;
        if (ptr->left_tree_ != nullptr && ptr->right_tree_ != nullptr)
        {
            // 找出右子树最小值替换
            Node* p = ptr->right_tree_;
            while(p->left_tree_)
            {
                p = p->left_tree_;
            }
            ptr->data_ = p->data_;
            // 替换之后，ptr就不重要了，处理好p的节点关系即可，所以直接复用变量
            ptr = p;
        }

        // 删除ptr的指针，逻辑一致了
        Node* pa = ptr->parent_;
        Node* child = ptr->left_tree_ == nullptr ? ptr->right_tree_ : ptr->left_tree_;

        if (child != nullptr) child->parent_ = pa;
        // 最后再处理 pa， 真的很妙
        if (pa == nullptr)
        {
            *pptr = child;
        }
        else
        {
            if (pa->left_tree_ == ptr)
            {
                pa->left_tree_ = child;
            }
            else
            {
                pa->right_tree_ = child;
            }
        }

        free_node(ptr);

        return true;
    }

    static void _InOrder(Node* ptr)
    {
        if (ptr)
        {
            _InOrder(ptr->left_tree_);
            stream << ptr->data_ << " ";
            _InOrder(ptr->right_tree_);
        }
    }

    void destory(Node* ptr)
    {
        if (ptr)
        {
            destory(ptr->left_tree_);
            destory(ptr->right_tree_);
            free_node(ptr);
        }
    }

    // 写代码就像运输，如果你提前把背包打开，那么就要拿着打开的背包赶路了
    static Node* find(Node* ptr, const T& val)
    {
        while(ptr != nullptr && alg::neq(ptr->data_, val))
        {
            ptr = alg::le(val, ptr->data_) ? ptr->left_tree_ : ptr->right_tree_;
        }

        // if(ptr == nullptr) return ptr;

        // for (;;)
        // {
        //     if (alg::le(val, ptr->data_))
        //     {
        //         ptr = ptr->left_tree_;
        //         if (ptr == nullptr) break;
        //     }
        //     else if (alg::le(ptr->data_, val))
        //     {
        //         ptr = ptr->right_tree_;
        //         if (ptr == nullptr) break;
        //     }
        //     else
        //     {
        //         break;
        //     }
        // }

        return ptr;
    }

    Node* first() const
    {
        return _first(_m_impl._root);

    }

    static Node* _first(Node* ptr)
    {
        if (ptr)
        {
            while(ptr->left_tree_)
            {
                ptr = ptr->left_tree_;
            }
        }

        return ptr;
    }

    static Node* next(Node* ptr)
    {
        if (ptr == nullptr) return ptr;

        if (ptr->right_tree_)
        {
            return _first(ptr->right_tree_);
        }
        else
        {
            // 真的不知道这样的代码是怎么写出来的。。服
            Node* pa = ptr->parent_;
            while(pa != nullptr && pa->left_tree_ != ptr)
            {
                ptr = pa;
                pa = pa->parent_;
            }
            return pa;

            // if (ptr->parent_ == nullptr) return nullptr;

            // if (alg::le(ptr->parent_->data_, ptr->data_))
            // {
            //     if (ptr->parent_->parent_ == nullptr) return nullptr;
                
            //     if (alg::le(ptr->parent_->parent_->data_, ptr->data_))
            //     {
            //         return nullptr;
            //     }
            //     else
            //     {
            //         return ptr->parent_->parent_;
            //     }
            // }
            // else
            // {
            //     return ptr->parent_;
            // }
        }
    }

    Node* last() const
    {
        return _last(_m_impl._root);
    }

    static Node* _last(Node* ptr)
    {
        if (ptr)
        {
            while(ptr->right_tree_)
            {
                ptr = ptr->right_tree_;
            }
        }

        return ptr;
    }

    static Node* prev(Node* ptr)
    {
        if (ptr == nullptr) return ptr;

        if (ptr->left_tree_)
        {
            return _last(ptr->left_tree_);
        }
        else
        {
            Node* pa = ptr->parent_;
            while(pa != nullptr && pa->right_tree_ != ptr)
            {
                ptr = pa;
                pa = pa->parent_;
            }
            return pa;
        }
    }

};

}


#endif