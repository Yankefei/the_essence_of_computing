#ifndef _TOOLS_BINARY_RB_TREE_RECURSIVE2_H_
#define _TOOLS_BINARY_RB_TREE_RECURSIVE2_H_

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

namespace rb_tree_recurs_2
{

template<typename T>
struct _RbNode
{
    _RbNode() = default;
    _RbNode(const T&val) : data_(val) {}
    _RbNode(const T&val, Color c) : data_(val), color_(c) {}

    _RbNode* left_tree_{nullptr};
    _RbNode* right_tree_{nullptr};
    _RbNode* parent_{nullptr};
    Color color_{Color::Black};
    T data_;
};


/*红黑树 递归*/
template<typename T, template <typename T1> class RbNode = _RbNode, typename Alloc = std::allocator<T>>
class RbTree : protected RbTree_Base<T, RbNode, Alloc>, protected RbTree_Util2<T, RbNode>
{
    typedef RbTree_Base<T, RbNode, Alloc> RbTreeBase;
    typedef typename RbTreeBase::Node Node;
    typedef Node*  Root;

    typedef RbTree_Util2<T, RbNode> RbTreeUtil2;

public:
    using RbTreeBase::buy_node;
    using RbTreeBase::free_node;
    using RbTreeBase::_m_impl;

    // 算法函数
    using RbTreeUtil2::handle_before_remove;
    using RbTreeUtil2::search_for_delete_min2;
    using RbTreeUtil2::search_for_delete_max2;

    using RbTreeUtil2::SignalRotateLeft;
    using RbTreeUtil2::SignalRotateRight;
    using RbTreeUtil2::doubleRotateLeft;
    using RbTreeUtil2::doubleRotateRight;
    using RbTreeUtil2::SignalRotateRightForRemove;
    using RbTreeUtil2::ChangeColorAfterRemoveRightRotate;
    using RbTreeUtil2::SignalRotateLeftForRemove;
    using RbTreeUtil2::ChangeColorAfterRemoveLeftRotate;
    using RbTreeUtil2::doubleRotateRightForRemove;
    using RbTreeUtil2::doubleRotateLeftForRemove;

    using RbTreeUtil2::get_color;
    using RbTreeUtil2::_get_hight;
    using RbTreeUtil2::_InOrder;

    using RbTreeUtil2::first;
    using RbTreeUtil2::next;
    using RbTreeUtil2::last;
    using RbTreeUtil2::prev;

public:
    RbTree()
    {
        initialize();
    }

    ~RbTree()
    {
        destory(_m_impl._root);
        _m_impl._root = nullptr;
    }

    // 自顶向下插入 递归
    bool insert(const T& val)
    {
        Node* ret_ptr = nullptr;
        bool res = insert(_m_impl._root->right_tree_, val, &ret_ptr);

        ret_ptr->color_ = Color::Black;
        _m_impl._root->right_tree_ = ret_ptr;
        return res;
    }

    // 自顶向下删除 递归
    bool remove(const T& val)
    {
        handle_before_remove(_m_impl._root->right_tree_, _m_impl._root, val);
        bool res = remove(&_m_impl._root->right_tree_, val);
        
        if (_m_impl._root->right_tree_)
            _m_impl._root->right_tree_->color_ = Color::Black;
        return res;
    }

    bool is_rb_tree()
    {   
        Node* read_root = _m_impl._root->right_tree_;
        if (read_root == nullptr) return true;

        if (read_root->color_ == Color::Red) return false;

        return _is_rb_tree(read_root).first;
    }

    int32_t get_hight()
    {
        return _get_hight(_m_impl._root->right_tree_);
    }

    Node* get_root()
    {
        return _m_impl._root->right_tree_;
    }

    void in_order()
    {
        _InOrder(_m_impl._root->right_tree_);
        stream << std::endl;
    }

    // 返回元素个数
    size_t NiceInOrder() const
    {
        size_t e_size = 0;
        Node* ptr = first(_m_impl._root->right_tree_);
        T f_min = ptr->data_;
        for (; ptr != nullptr; ptr = next(ptr))
        {
            if (alg::le(ptr->data_, f_min))
            {
                assert(false);
            }
            f_min = ptr->data_;
            // stream << ptr->data_ << " ";
            e_size ++;
        }
        stream << std::endl;
        return e_size;
    }

    // 返回元素个数
    size_t ResNiceInOrder() const
    {
        size_t e_size = 0;
        Node* ptr = last(_m_impl._root->right_tree_);
        T f_max = ptr->data_;
        for (; ptr != nullptr; ptr = prev(ptr))
        {
            if (alg::le(f_max, ptr->data_))
            {
                assert(false);
            }
            f_max = ptr->data_;
            // stream << ptr->data_ << " ";
            e_size ++;
        }
        stream << std::endl;
        return e_size;
    }

    void print_tree()
    {
        draw_tree<Node>(_m_impl._root->right_tree_);
    }

private:
    bool insert(Node* ptr, const T&val, Node** ret_pptr)
    {
        if (ptr == nullptr)
        {
            *ret_pptr = buy_node(val, Color::Red);
            return true;
        }
    
        // 在递归调用之前检查节点的分裂，程序处理的方向就是自顶向下
        if (get_color(ptr->left_tree_) == Color::Red &&
            get_color(ptr->right_tree_) == Color::Red)
        {
            ptr->color_ = Color::Red;
            ptr->left_tree_->color_ = Color::Black;
            ptr->right_tree_->color_ = Color::Black;
        }

        bool res = false;
        Node* ret_ptr = ptr;
        do
        {
            if (alg::le(val, ptr->data_))
            {
                ret_ptr = ptr->left_tree_;
                res = insert(ret_ptr, val, &ret_ptr);

                if (ret_ptr != ptr->left_tree_)
                {
                    ptr->left_tree_ = ret_ptr;
                    ret_ptr->parent_ = ptr; // 维护父指针
                }
                
                if (ret_ptr->color_ == Color::Red)
                {
                    if (ret_ptr->left_tree_ && ret_ptr->left_tree_->color_ == Color::Red)
                    {
                        // 单循环
                        ret_ptr = SignalRotateLeft(ptr);
                    }
                    else if (ret_ptr->right_tree_ && ret_ptr->right_tree_->color_ == Color::Red)
                    {
                        // 双循环
                        ret_ptr = doubleRotateLeft(ptr);
                    }
                }

                // ret_ptr完成了对下层的所有重组逻辑后，替换ptr返回
                if (ptr->left_tree_ == ret_ptr)
                    ret_ptr = ptr;
            }
            else if (alg::le(ptr->data_, val))
            {
                ret_ptr = ptr->left_tree_;
                res = insert(ptr->right_tree_, val, &ret_ptr);

                if (ret_ptr != ptr->right_tree_)
                {
                    ptr->right_tree_ = ret_ptr;
                    ret_ptr->parent_ = ptr; // 维护父指针
                }

                if (ret_ptr->color_ == Color::Red)
                {
                    if (ret_ptr->left_tree_ && ret_ptr->left_tree_->color_ == Color::Red)
                    {
                        ret_ptr = doubleRotateRight(ptr);
                    }
                    else if (ret_ptr->right_tree_ && ret_ptr->right_tree_->color_ == Color::Red)
                    {
                        ret_ptr = SignalRotateRight(ptr);
                    }
                }

                if (ptr->right_tree_ == ret_ptr)
                {
                    ret_ptr = ptr; // 正常返回 ptr
                }
            }
        } while (false);

        *ret_pptr = ret_ptr;

        return res;
    }

    bool remove(Node** _ptr, const T& val)
    {
        Node* ptr = *_ptr;
        if (ptr == nullptr) return false;

        bool res = false;
        Node* ret_ptr = nullptr;
        Node* pptr = nullptr;
        if (alg::le(val, ptr->data_))
        {
            // 根据是否旋转，判断下一个值应该如何传递，同时保证引用形参的正确性
            res = search_for_delete_min2(ptr, val, false);
            ret_ptr = ptr;
            if (res)
            {
                pptr = ptr->left_tree_;
                ptr = pptr->left_tree_;
                res = remove(&ptr, val);
            }
            else
            {
                pptr = ptr;
                ptr = pptr->left_tree_;
                res = remove(&ptr, val);
            }
            pptr->left_tree_ = ptr;
            *_ptr = ret_ptr;
            return res;
        }
        else if (alg::le(ptr->data_, val))
        {
            res = search_for_delete_max2(ptr, val, false);   // 仅依据数据来转向
            ret_ptr = ptr;
            if (res)
            {
                pptr = ptr->right_tree_;
                ptr = pptr->right_tree_;
                res = remove(&ptr, val);
            }
            else
            {
                pptr = ptr;
                ptr = pptr->right_tree_;
                res = remove(&ptr, val);
            }
            pptr->right_tree_ = ptr;
            *_ptr = ret_ptr;
            return res;
        }
        else
        {
            Node* old_ptr = ptr;
            Dir dir_change = Dir::Unknown; // 控制当前节点与父指针的方向
            if ((ptr->left_tree_ != nullptr && ptr->right_tree_ != nullptr) ||
                ptr->right_tree_ != nullptr)
            {
                // 这里处理之后，会发生遍历线路的拐弯，需要特殊处理
                res = search_for_delete_max2(ptr, val, true, true); // 强制转向
                ret_ptr = ptr;
                if (res)
                    ptr = ptr->right_tree_->right_tree_;
                else
                    ptr = ptr->right_tree_;

                pptr = ptr->parent_;
                while(ptr->left_tree_)
                {
                    res = search_for_delete_min2(ptr, val, true);  // 去掉数据和外界的强制转向
                    if (res)
                    {
                        if (dir_change == Dir::Unknown)
                        {
                            pptr->right_tree_ = ptr;
                            dir_change = Dir::Left;
                        }
                        else
                        {
                            pptr->left_tree_ = ptr;
                        }
                        ptr = ptr->left_tree_->left_tree_;
                    }
                    else
                    {
                        if (dir_change == Dir::Unknown)
                            dir_change = Dir::Left;
                        ptr = ptr->left_tree_;
                    }
                    pptr = ptr->parent_; // 更新pptr的值
                }
                old_ptr->data_ = ptr->data_;

                // 将删除目标设置为ptr节点
                res = remove(&ptr, ptr->data_);
                // 最后还是更新父节点的左子树，左子树为更新后的值
                dir_change == Dir::Unknown ?
                    pptr->right_tree_ = ptr : pptr->left_tree_ = ptr;
                *_ptr = ret_ptr;
                return res;
            }
            else if (ptr->left_tree_ != nullptr)
            {
                res = search_for_delete_min2(ptr, val, true, true);
                ret_ptr = ptr;
                if (res)
                    ptr = ptr->left_tree_->left_tree_;
                else
                    ptr = ptr->left_tree_;

                pptr = ptr->parent_;
                while(ptr->right_tree_) // 当右子节点存在时，再进行接下来的处理
                {
                    res = search_for_delete_max2(ptr, val, true);
                    if (res)
                    {
                        if (dir_change == Dir::Unknown)
                        {
                            pptr->left_tree_ = ptr;
                            dir_change = Dir::Right;
                        }
                        else
                        {
                            pptr->right_tree_ = ptr; // 更新变更的ptr指针的父节点
                        }
                        ptr = ptr->right_tree_->right_tree_;
                    }
                    else
                    {
                        if (dir_change == Dir::Unknown)
                            dir_change = Dir::Right;
                        ptr = ptr->right_tree_;
                    }
                    pptr = ptr->parent_; // 更新pptr的值
                }
                old_ptr->data_ = ptr->data_;
                // 将删除目标设置为ptr节点
                res = remove(&ptr, ptr->data_);
                    // 最后还是更新父节点的左子树，左子树为更新后的值
                dir_change == Dir::Unknown ?
                    pptr->left_tree_ = ptr : pptr->right_tree_ = ptr;
                *_ptr = ret_ptr;
                return res;
            }
            else
            {
                if (ptr != _m_impl._root->right_tree_)
                    assert(ptr->color_ == Color::Red);
                free_node(ptr);
                *_ptr = nullptr;
                return true;
            }
        }
    }

    using RbInfo = std::pair<bool/*is rb_tree ?*/, uint32_t/*size of black_node*/>;
    RbInfo _is_rb_tree(Node* ptr)
    {
        if (ptr == nullptr) return RbInfo(true, 0);

        RbInfo res(false, 0);
        Color c = ptr->color_;

        do
        {
            if (c == Color::Red)
            {
                if ((ptr->right_tree_ && ptr->right_tree_->color_ == c) ||
                    (ptr->left_tree_ && ptr->left_tree_->color_ == c))
                {
                    break;
                }
            }

            RbInfo l_info = _is_rb_tree(ptr->left_tree_);
            if (!l_info.first) break;
            RbInfo r_info = _is_rb_tree(ptr->right_tree_);
            if (!r_info.first) break;

            if (l_info.second != r_info.second) break;

            bool is_sort = true;
            if (ptr->left_tree_)
                is_sort &= alg::le(ptr->left_tree_->data_, ptr->data_);
            if (ptr->right_tree_)
                is_sort &= alg::le(ptr->data_, ptr->right_tree_->data_);

            res.first = is_sort;
            res.second = l_info.second + (c == Color::Black ? 1 : 0);
        } while (false);

        // if (ptr->data_ == T(140))
        // {
        //     draw_tree<Node>(ptr);
        // }
        if (!res.first)
        {
            draw_tree<Node>(ptr);
            assert(false);
        }

        return res;
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

    void initialize()
    {
        if (null_node_ == nullptr)
            null_node_ = buy_node(T());

        if (_m_impl._root == nullptr)
        {
            _m_impl._root = buy_node(T());
            _m_impl._root->left_tree_ = null_node_;
        }
    }


private:
    /*static*/ Node* null_node_{nullptr};
};

// template<typename T, template <typename T1> class RbNode, typename Alloc>
// typename RbTree<T, RbNode, Alloc>::Node*  RbTree<T, RbNode, Alloc>::null_node_;


}
}

#endif