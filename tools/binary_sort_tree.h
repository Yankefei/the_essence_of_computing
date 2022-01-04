#ifndef _TOOLS_BINARY_SORT_TREE2_H_
#define _TOOLS_BINARY_SORT_TREE2_H_

#include <memory>
#include <cassert>

#include "algorithm.hpp"

namespace tools
{

template<typename T>
struct _SNode
{
    _SNode() = default;
    _SNode(const T&val) : data_(val) {}

    _SNode* left_tree_{nullptr};
    _SNode* right_tree_{nullptr};
    T data_;
};


template<typename T, typename Alloc>
class BsTree_Base
{
public:
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<_SNode<T>> _SNode_alloc_type;

    typedef typename std::allocator_traits<_SNode_alloc_type>
        rebind_traits;

    typedef typename rebind_traits::pointer  pointer;

    typedef _SNode<T>        Node;

public:
    struct Base_Impl : public _SNode_alloc_type
    {
        Base_Impl(): _SNode_alloc_type(), _root()
        {
        }

        ~Base_Impl() {}

        Base_Impl(Base_Impl& _x) noexcept
        {
            std::swap(_root, _x._root);
        }

        Node*     _root{nullptr};
    };

    BsTree_Base() {}
    ~BsTree_Base() {} // 设置虚函数，Base_Impl 会额外增加8字节内存

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

/*排序二叉树*/
template<typename T, typename Alloc = std::allocator<T>>
class BsTree : protected BsTree_Base<T, Alloc>
{
    typedef typename BsTree_Base<T, Alloc>::Node Node;
    typedef Node*  Root;

public:
    using BsTree_Base<T, Alloc>::buy_node;
    using BsTree_Base<T, Alloc>::free_node;
    using BsTree_Base<T, Alloc>::_m_impl;

public:
    BsTree() {}
    ~BsTree() { destory(_m_impl._root); }

    // 插入重复数据则使用替换操作
    bool insert(const T& val)
    {
        return insert(_m_impl._root, val);
    }

    bool insert2(const T& val)
    {
        bool res = false;

        return res;
    }

    bool remove(const T& val)
    {
        return remove(_m_impl._root, val);
    }

    bool remove2(const T& val)
    {

    }

    void InOrder()
    {
        _InOrder(_m_impl._root);
        stream << std::endl;
    }

    void NiceInOrder()
    {

    }

private:
    bool insert(Node*& ptr, const T& val)
    {
        if(ptr == nullptr)
        {
            ptr = buy_node(val);
        }
        else
        {
            if (alg::gt(ptr->data_, val))
            {
                insert(ptr->left_tree_, val);
            }
            else if (alg::le(ptr->data_, val))
            {
                insert(ptr->right_tree_, val);
            }
            else
            {
                ptr->data_ = val;
            }
        }
        return true;
    }

    bool remove(Node*& ptr, const T& val)
    {
        if (ptr == nullptr) return false;

        if (alg::gt(ptr->data_, val))
        {
            return remove(ptr->left_tree_, val);
        }
        else if (alg::le(ptr->data_, val))
        {
            return remove(ptr->right_tree_, val);
        }
        else
        {
            // 2中情况，一种是左右子树均在，一种是其他
            if (ptr->left_tree_ && ptr->right_tree_)
            {
                auto r_min = find_min(ptr->right_tree_);
                ptr->data_ = r_min->data_;
                remove(ptr->right_tree_, r_min->data_);
            }
            else
            {
                auto temp = ptr;
                if (ptr->left_tree_ == nullptr)
                {
                    ptr = ptr->right_tree_;
                }
                else if (ptr->right_tree_ == nullptr)
                {
                    ptr = ptr->left_tree_;
                }

                free_node(temp);
            }
            
            return true;
        }
    }

    bool remove_min()
    {

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

    void destory(Node* ptr)
    {
        if (ptr)
        {
            destory(ptr->left_tree_);
            destory(ptr->right_tree_);
            free_node(ptr);
        }
    }

    Node* find(Node* ptr, const T& val)
    {

    }

    Node* find_max(Node* ptr)
    {
        // 先判断ptr, 再while循环处理 ptr->right_tree_
        if (!ptr)
        {
            while(ptr->right_tree_)
            {
                ptr = ptr->right_tree_;
            }
        }

        return ptr;
    }

    Node* find_min(Node* ptr)
    {
        if (!ptr)
        {
            while(ptr->left_tree_)
            {
                ptr = ptr->left_tree_;
            }
        }

        return ptr;
    }
};


}

#endif