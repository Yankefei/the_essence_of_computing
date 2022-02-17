#ifndef _TOOLS_BINARY_SORT_TREE2_H_
#define _TOOLS_BINARY_SORT_TREE2_H_

#include <memory>
#include <cassert>

#include "binary_sort_tree_base.h"
#include "algorithm.hpp"

namespace tools
{

template<typename T>
struct _SNode2
{
    _SNode2() = default;
    _SNode2(const T&val) : data_(val) {}

    _SNode2* left_tree_{nullptr};
    _SNode2* right_tree_{nullptr};
    _SNode2* parent_{nullptr};
    T data_;
};

/*排序二叉树*/
template<typename T, template <typename T1> class SNode = _SNode2, typename Alloc = std::allocator<T>>
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

    bool insert(const T& val)
    {
        return insert(_m_impl._root, val);
    }

    bool remove(const T& val)
    {
        return remove(_m_impl._root, val);
    }

    void InOrder()
    {
        _InOrder(_m_impl._root);
        stream << std::endl;
    }

    void NiceInOrder()
    {
        auto ptr = first();
        for (; ptr != nullptr; ptr = next(ptr))
        {
            stream << ptr->data_ << " ";
        }
        stream << std::endl;
    }

    Node* find(const T& val)
    {
        return find(_m_impl._root, val);
    }

private:
    bool insert(Node*& ptr, const T& val)
    {
        if (ptr == nullptr)
        {
            ptr = buy_node(val);
        }
        else
        {
            Node* tmp = ptr;
            Node* f_ptr = nullptr;  // 充当父节点
            for(;;)
            {
                if (alg::le(val, tmp->data_))
                {
                    f_ptr = tmp;
                    tmp = tmp->left_tree_;

                    if (tmp == nullptr)
                    {
                        auto new_node = buy_node(val);
                        f_ptr->left_tree_ = new_node;
                        f_ptr->left_tree_->parent_ = f_ptr;
                        break;
                    }
                }
                else if (alg::le(tmp->data_, val))
                {
                    f_ptr = tmp;
                    tmp = tmp->right_tree_;

                    if (tmp == nullptr)
                    {
                        auto new_node = buy_node(val);
                        f_ptr->right_tree_ = new_node;
                        f_ptr->right_tree_->parent_ = f_ptr;
                        break;
                    }
                }
                else
                {
                    tmp->data_ = val;
                    break;
                }
            }
        }

        return true;
    }

    // 删除的过程需要维护 parent_ 指针
    bool remove(Node*& ptr, const T& val)
    {
        if (ptr == nullptr) return false;

        Node* tmp = ptr;
        int desc = 0; // 记录遍历的方向0:未知，1：左， 2：右
        for (;;)
        {
            if (alg::le(val, tmp->data_))
            {
                tmp = tmp->left_tree_;
                desc = 1;

                if (tmp == nullptr)
                    return false;
            }
            else if (alg::le(tmp->data_, val))
            {
                tmp = tmp->right_tree_;
                desc = 2;

                if (tmp == nullptr)
                    return false;
            }
            else
            {
                if (tmp->left_tree_ && tmp->right_tree_)
                {
                    auto l_max = remove_max(tmp->left_tree_);
                    tmp->data_ = l_max->data_;
                    free_node(l_max);
                }
                else
                {
                    if (tmp->left_tree_ == nullptr)
                    {
                        switch(desc)
                        {
                            case 0:
                            {
                                ptr = ptr->right_tree_;
                                if (ptr)
                                    ptr->parent_ = nullptr;
                                break;
                            }
                            case 1:
                            {
                                tmp->parent_->left_tree_ = tmp->right_tree_;
                                if (tmp->right_tree_)
                                    tmp->right_tree_->parent_ = tmp->parent_;
                                break;
                            }
                            case 2:
                            {
                                tmp->parent_->right_tree_ = tmp->right_tree_;
                                if (tmp->right_tree_)
                                    tmp->right_tree_->parent_ = tmp->parent_;
                                break;
                            }
                            default:
                                assert(false);
                                break;
                        }
                    }
                    else if (tmp->right_tree_ == nullptr)
                    {
                        switch(desc)
                        {
                            case 0:
                            {
                                ptr = ptr->left_tree_;
                                ptr->parent_ = nullptr;
                                break;
                            }
                            case 1:
                            {
                                tmp->parent_->left_tree_ = tmp->left_tree_;
                                if (tmp->left_tree_)
                                    tmp->left_tree_->parent_ = tmp->parent_;
                                break;
                            }
                            case 2:
                            {
                                tmp->parent_->right_tree_ = tmp->left_tree_;
                                if (tmp->left_tree_)
                                    tmp->left_tree_->parent_ = tmp->parent_;
                                break;
                            }
                            default:
                                assert(false);
                                break;
                        }
                    }

                    free_node(tmp);
                }

                return true;
            }
        }
    }

    Node* remove_min(Node*& ptr)
    {
        Node* tmp = nullptr;
        tmp = ptr;

        if (tmp == nullptr) return tmp;

        if (tmp->left_tree_)
        {
            while(tmp->left_tree_)
            {
                tmp = tmp->left_tree_;
            }
            tmp->parent_->left_tree_ = tmp->right_tree_;
            if (tmp->right_tree_)
                tmp->right_tree_->parent_ = tmp->parent_;
        }
        else
        {
            ptr = tmp->right_tree_;
            //assert(ptr->parent_ != tmp->parent_);
            if (ptr)
                ptr->parent_ = tmp->parent_;
        }

        return tmp;
    }

    Node* remove_max(Node*& ptr)
    {
        Node* tmp = nullptr;
        tmp = ptr;

        if (tmp == nullptr) return tmp;

        if (tmp->right_tree_)
        {
            while(tmp->right_tree_)
            {
                tmp = tmp->right_tree_;
            }
            tmp->parent_->right_tree_ = tmp->left_tree_;
            if (tmp->left_tree_)
                tmp->left_tree_->parent_ = tmp->parent_;
        }
        else
        {
            ptr = tmp->left_tree_;
            //assert(ptr->parent_ != tmp->parent_);
            if (ptr)
                ptr->parent_ = tmp->parent_;
        }

        return tmp;
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
        if(ptr == nullptr) return ptr;

        for (;;)
        {
            if (alg::le(val, ptr->data_))
            {
                ptr = ptr->left_tree_;
                if (ptr == nullptr) break;
            }
            else if (alg::le(ptr->data_, val))
            {
                ptr = ptr->right_tree_;
                if (ptr == nullptr) break;
            }
            else
            {
                break;
            }
        }

        return ptr;
    }

    Node* first()
    {
        auto ptr = _m_impl._root;
        if (ptr)
        {
            while(ptr->left_tree_)
            {
                ptr = ptr->left_tree_;
            }
        }

        return ptr;
    }

    Node* next(Node* ptr)
    {
        if (ptr == nullptr) return ptr;

        if (ptr->right_tree_)
        {
            return find_right_max(ptr->right_tree_);
        }
        else
        {
            if (ptr->parent_ == nullptr) return nullptr;

            if (alg::le(ptr->parent_->data_, ptr->data_))
            {
                if (ptr->parent_->parent_ == nullptr) return nullptr;
                
                if (alg::le(ptr->parent_->parent_->data_, ptr->data_))
                {
                    return nullptr;
                }
                else
                {
                    return ptr->parent_->parent_;
                }
            }
            else
            {
                return ptr->parent_;
            }
        }
    }

    Node* find_right_max(Node* ptr)
    {
        if (ptr == nullptr) return ptr;
        
        if (ptr)
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