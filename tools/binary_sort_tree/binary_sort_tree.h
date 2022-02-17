#ifndef _TOOLS_BINARY_SORT_TREE2_H_
#define _TOOLS_BINARY_SORT_TREE2_H_

#include <memory>
#include <cassert>

#include "binary_sort_tree_base.h"

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


/*排序二叉树*/
template<typename T, template <typename T1> class SNode = _SNode, typename Alloc = std::allocator<T>>
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

    // 插入重复数据则使用替换操作
    bool insert(const T& val)
    {
        return insert(_m_impl._root, val);
    }

    // 非递归
    bool insert2(const T& val)
    {
        return insert2(_m_impl._root, val);
    }

    bool remove(const T& val)
    {
        return remove(_m_impl._root, val);
    }

    // 非递归
    bool remove2(const T& val)
    {
        return remove2(_m_impl._root, val);
    }

    void InOrder()
    {
        _InOrder(_m_impl._root);
        stream << std::endl;
    }

    Node* find(const T& val)
    {
        return find(_m_impl._root, val);
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
            if (alg::le(val, ptr->data_))
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

    // 第一个参数采用引用少不了
    bool insert2(Node*& ptr, const T& val)
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
                        f_ptr->left_tree_ = buy_node(val);
                        break;
                    }
                }
                else if (alg::le(tmp->data_, val))
                {
                    f_ptr = tmp;
                    tmp = tmp->right_tree_;

                    if (tmp == nullptr)
                    {
                        f_ptr->right_tree_ = buy_node(val);
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


    bool remove(Node*& ptr, const T& val)
    {
        if (ptr == nullptr) return false;

        if (alg::le(val, ptr->data_))
        {
            return remove(ptr->left_tree_, val);
        }
        else if (alg::le(ptr->data_, val))
        {
            return remove(ptr->right_tree_, val);
        }
        else
        {
            // 2种情况，一种是左右子树均在，一种是其他
            if (ptr->left_tree_ && ptr->right_tree_)
            {
                // auto r_min = find_min(ptr->right_tree_);
                // ptr->data_ = r_min->data_;
                // remove(ptr->right_tree_, r_min->data_);

                // 查找和删除同步进行
                // 可以查找右子树的最小一个，或者左子树的最大一个
                //auto r_min = remove_min(ptr->right_tree_);
                auto r_min = remove_max(ptr->left_tree_);
                ptr->data_ = r_min->data_;
                free_node(r_min);
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

    // 第一个参数采用引用少不了
    bool remove2(Node*& ptr, const T& val)
    {
        if (ptr == nullptr) return false;

        Node* tmp = ptr;
        Node* f_ptr = nullptr;   // 充当父节点
        int desc = 0;     // 记录遍历的方向0:未知，1：左， 2：右
        for (;;)
        {
            if (alg::le(val, tmp->data_))
            {
                f_ptr = tmp;
                tmp = tmp->left_tree_;
                desc = 1;

                if (tmp == nullptr)
                    return false;
            }
            else if (alg::le(tmp->data_, val))
            {
                f_ptr = tmp;
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
                                break;
                            }
                            case 1:
                            {
                                f_ptr->left_tree_ = tmp->right_tree_;
                                break;
                            }
                            case 2:
                            {
                                f_ptr->right_tree_ = tmp->right_tree_;
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
                                break;
                            }
                            case 1:
                            {
                                f_ptr->left_tree_ = tmp->left_tree_;
                                break;
                            }
                            case 2:
                            {
                                f_ptr->right_tree_ = tmp->left_tree_;
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

    // 查找到子树最小的一个，删除并返回
    Node* remove_min(Node*& ptr)
    {
        Node* tmp = nullptr;
        tmp = ptr;

        if (!tmp) return tmp;

        if (tmp->left_tree_)
        {
            Node* f_ptr = tmp;  // 作为一个临时的parent节点

            while(tmp->left_tree_)
            {
                f_ptr = tmp;
                tmp = tmp->left_tree_;
            }

            f_ptr->left_tree_ = tmp->right_tree_;
        }
        else
        {
            ptr = tmp->right_tree_;
        }

        return tmp;
    }

    // 查找到子树最大的一个，删除并返回
    Node* remove_max(Node*& ptr)
    {
        Node* tmp = nullptr;
        tmp = ptr;

        if (!tmp) return tmp;

        if (tmp->right_tree_)
        {
            Node* f_ptr = tmp;  // 作为一个临时的parent节点

            while(tmp->right_tree_)
            {
                f_ptr = tmp;
                tmp = tmp->right_tree_;
            }

            f_ptr->right_tree_ = tmp->left_tree_;
        }
        else
        {
            ptr = tmp->left_tree_;
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