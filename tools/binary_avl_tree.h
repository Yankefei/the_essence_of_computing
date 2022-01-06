#ifndef _TOOLS_BINARY_AVL_TREE_H_
#define _TOOLS_BINARY_AVL_TREE_H_

#include <memory>
#include <cassert>

#include "queue.h"
#include "algorithm.hpp"

namespace tools
{

template<typename T>
struct _BNode
{
    _BNode() = default;
    _BNode(const T&val) : data_(val) {}

    _BNode* left_tree_{nullptr};
    _BNode* right_tree_{nullptr};
    int32_t hight_{-1};    // 树高
    T data_;
};


template<typename T, typename Alloc>
class AvlTree_Base
{
public:
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<_BNode<T>> _BNode_alloc_type;

    typedef typename std::allocator_traits<_BNode_alloc_type>
        rebind_traits;

    typedef typename rebind_traits::pointer  pointer;

    typedef _BNode<T>        Node;

public:
    struct Base_Impl : public _BNode_alloc_type
    {
        Base_Impl(): _BNode_alloc_type(), _root()
        {
        }

        ~Base_Impl() {}

        Base_Impl(Base_Impl& _x) noexcept
        {
            std::swap(_root, _x._root);
        }

        Node*     _root{nullptr};
    };

    AvlTree_Base() {}
    ~AvlTree_Base() {} // 设置虚函数，Base_Impl 会额外增加8字节内存

public:
    Base_Impl   _m_impl;

    Node* buy_node(const T& val)
    {
        pointer p = rebind_traits::allocate(_m_impl, 1);
        rebind_traits::construct(_m_impl, p, val);
        return static_cast<Node*>(p);
    }

    void free_node(Node* ptr)
    {
        rebind_traits::destroy(_m_impl, ptr);
        rebind_traits::deallocate(_m_impl, ptr, 1);
    }
};

/*排序平衡二叉树*/
template<typename T, typename Alloc = std::allocator<T>>
class AvlTree : protected AvlTree_Base<T, Alloc>
{
    typedef typename AvlTree_Base<T, Alloc>::Node Node;
    typedef Node*  Root;

public:
    using AvlTree_Base<T, Alloc>::buy_node;
    using AvlTree_Base<T, Alloc>::free_node;
    using AvlTree_Base<T, Alloc>::_m_impl;

public:
    AvlTree() {}
    ~AvlTree() { destory(_m_impl._root); }

    // 重复元素插入失败
    bool insert(const T& val)
    {
        return insert(_m_impl._root, val);
    }

    bool insert2(const T& val)
    {
        return insert2(&_m_impl._root, val);
    }

    bool remove(const T& val)
    {

    }

    bool remove2(const T& val)
    {

    }

    void InOrder()
    {
        _InOrder(_m_impl._root);
        stream << std::endl;
    }

    void LevelOrder()
    {
        _LevelOrder(_m_impl._root);
    }

    Root get_root()
    {
        return _m_impl._root;
    }

private:
    // 递归
    bool insert(Node*& ptr, const T& val)
    {
        if (ptr == nullptr)
        {
            ptr = buy_node(val);
            ptr->hight_ = 0;
            return true;
        }

        if (alg::gt(ptr->data_, val))
        {
            if (!insert(ptr->left_tree_, val)) return false;

            if (hight(ptr->left_tree_) - hight(ptr->right_tree_) == 2)
            {
                // 调整之后，必须保证当前的的子树内部高度已经保持正常
                // 1. 保证平衡， 2. 节点高度是正确的
                if (alg::gt(ptr->left_tree_->data_, val))
                {
                    signal_rotate_withleft(&ptr);
                }
                else
                {
                    double_rotate_withleft(&ptr);
                }
            }
        }
        else if (alg::le(ptr->data_, val))
        {
            if (!insert(ptr->right_tree_, val)) return false;

            if (hight(ptr->right_tree_) - hight(ptr->left_tree_) == 2)
            {
                if (alg::le(ptr->right_tree_->data_, val))
                {
                    signal_rotate_withright(&ptr);
                }
                else
                {
                    double_rotate_withright(&ptr);
                }
            }
        }
        else
        {
            return false;
        }

        // 这里计算的结果是当前节点的高度值
        ptr->hight_ = alg::max(hight(ptr->left_tree_), hight(ptr->right_tree_)) + 1;

        return true;
    }

    //  非递归
    bool insert(Node** pptr, const T& val)
    {
        if (pptr == nullptr) false;

        Node* ptr = *pptr;
        while(ptr != nullptr && alg::neq(ptr->data_, val))
        {
            ptr = alg::gt(ptr->data_, val) ? ptr->left_tree_ : ptr->right_tree_;
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

    static void signal_rotate_withleft(Node** pptr)
    {
        Node* k2 = *pptr;
        Node* k1 = k2->left_tree_;
        k2->left_tree_ = k1->right_tree_;
        k1->right_tree_ = k2;

        // 处理高度
        k2->hight_ = alg::max(hight(k2->left_tree_), hight(k2->right_tree_)) + 1;
        k1->hight_ = alg::max(hight(k1->left_tree_), k2->hight_) + 1;

        *pptr = k1;
    }

    static void signal_rotate_withright(Node** pptr)
    {
        Node* k1 = *pptr;
        Node* k2 = k1->right_tree_;
        k1->right_tree_ = k2->left_tree_;
        k2->left_tree_ = k1;

        k1->hight_ = alg::max(hight(k1->left_tree_), hight(k1->right_tree_)) + 1;
        k2->hight_ = alg::max(hight(k2->right_tree_), k1->hight_) + 1;

        *pptr = k2;
    }

    static void double_rotate_withleft(Node** pptr)
    {
        signal_rotate_withright(&(*pptr)->left_tree_);
        signal_rotate_withleft(pptr);
    }

    static void double_rotate_withright(Node** pptr)
    {
        signal_rotate_withleft(&(*pptr)->right_tree_);
        signal_rotate_withright(pptr);
    }

    static Node* find_max(Node* ptr)
    {

    }

    static Node* find_min(Node* ptr)
    {

    }

    static int32_t hight(Node* ptr)
    {
        if (ptr == nullptr)
            return -1;
        else
            return ptr->hight_;
    }

    void _InOrder(Node* ptr)
    {
        if (ptr)
        {
            _InOrder(ptr->left_tree_);
            stream << ptr->data_ << " ";
            _InOrder(ptr->right_tree_);
        }
    }

    void _LevelOrder(Node* ptr)
    {

    }
};

}

#endif