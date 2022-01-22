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

    using RbTreeUtil2::_is_rb_tree;
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
        if (_m_impl._root->right_tree_ != null_node_)
            destory(_m_impl._root->right_tree_);
        free_node(_m_impl._root);
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
            if (alg::gt(f_min, ptr->data_))
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
            if (alg::gt(ptr->data_, val))
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
        if (alg::gt(ptr->data_, val))
        {
            // 根据是否旋转，判断下一个值应该如何传递，同时保证引用形参的正确性
            res = search_for_delete_min2(ptr, val, false);
            ret_ptr = ptr;
            if (res)
            {
                pptr = ptr->left_tree_;
                ptr = ptr->left_tree_->left_tree_;
                res = remove(&ptr, val);
            }
            else
            {
                pptr = ptr;
                ptr = ptr->left_tree_;
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
                ptr = ptr->right_tree_->right_tree_;
                res = remove(&ptr, val);
            }
            else
            {
                pptr = ptr;
                ptr = ptr->right_tree_;
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

    // 提前处理根节点的特殊情况
    // 需要处理的情况是: 根据要删除的值，来选择将根节点旋转向何方
    // pptr的参数可以去掉，因为该值只会是 _m_impl.root_, 为了模块化，所以增加pptr参数
    void handle_before_remove(Node*& ptr, Node* pptr, const T& val)
    {
        if (ptr == nullptr) return;

        if (ptr->left_tree_ != nullptr && ptr->right_tree_ != nullptr &&
            ptr->left_tree_->color_ == Color::Red && ptr->right_tree_->color_ == Color::Red)
        {
            return;
        }

        Dir dir = alg::gt(ptr->data_, val) ? Dir::Left : Dir::Right;

        if (dir == Dir::Right && ptr->left_tree_ != nullptr  &&
            ptr->left_tree_->color_ == Color::Red)
        {
            ptr->left_tree_->color_ = Color::Black;
            ptr = SignalRotateLeftForRemove(ptr);
            pptr->right_tree_ = ptr;
            ptr->right_tree_->color_ = Color::Red;
        }

        if (dir == Dir::Left && ptr->right_tree_ != nullptr &&
            ptr->right_tree_->color_ == Color::Red)
        {
            ptr->right_tree_->color_ = Color::Black;
            ptr = SignalRotateRightForRemove(ptr);
            pptr->right_tree_ = ptr;
            ptr->left_tree_->color_ = Color::Red;
        }
    }

    // 搜索儿子节点中最小节点，并将最小节点的颜色修改为红色
    // 返回旋转与否的 bool 值
    // lock_dir 表示是否锁定方向，锁定表示是否修改向下搜索的方向
    // is_change_dir 表示是否强制修改原本向下搜索的方向
    bool search_for_delete_min2(Node*& ptr, const T& val, bool lock_dir, bool is_change_dir = false)
    {
        if (ptr == nullptr || ptr->left_tree_ == nullptr) return false;

        bool res = false;
        do
        {
            Node* l_ptr = ptr->left_tree_;
            if (l_ptr->color_ == Color::Red)
                break;  // 如果左子节点已经是红色, 直接返回

            if (l_ptr->left_tree_ != nullptr && l_ptr->right_tree_ != nullptr &&
                l_ptr->left_tree_->color_ == Color::Red && l_ptr->right_tree_->color_ == Color::Red)
            {
                break;
            }

            Dir dir = alg::gt(l_ptr->data_, val) ? Dir::Left : Dir::Right;
            // 需要根据遍历顺序后续是否转向，来调整遍历的方向

            bool change_dir = is_change_dir || (!lock_dir && dir == Dir::Right);
            // 由数据或者函数参数驱动转向, 由正常到非正常转向，需要干预
            if (l_ptr->left_tree_ != nullptr  && l_ptr->left_tree_->color_ == Color::Red)
            {
                if (change_dir)
                {
                    l_ptr->left_tree_->color_ = Color::Black;
                    l_ptr = SignalRotateLeftForRemove(l_ptr);
                    ptr->left_tree_ = l_ptr;
                    l_ptr->right_tree_->color_ = Color::Red;
                }
                break;
            }

            // 仅由数据驱动转向   由非正常到正常转向，一般不需要干预
            if (l_ptr->right_tree_ != nullptr && l_ptr->right_tree_->color_ == Color::Red)
            {
                if (change_dir)
                    break;

                l_ptr->right_tree_->color_ = Color::Black;
                l_ptr = SignalRotateRightForRemove(l_ptr);
                ptr->left_tree_ = l_ptr;
                l_ptr->left_tree_->color_ = Color::Red;

                break;
            }

            // 只要左子节点为黑，那么如果父节点非空，则右子节点必须是非空的, 否则违反了rb树的性质
            Node* r_ptr = ptr->right_tree_;
            assert(r_ptr != nullptr);

            Color r_l_c = get_color(r_ptr->left_tree_);
            Color r_r_c = get_color(r_ptr->right_tree_);
            // 对于根节点，也可以使用该算法进行处理，做复用，但是最后必须将新跟节点的值设置为 Black
            // 父节点是红色，右子树只能是黑色
            if (r_l_c == Color::Black && r_r_c == Color::Black)
            {
                // 将父节点，左右子节点共同组成一个4-节点
                ptr->color_ = Color::Black;
                l_ptr->color_ = Color::Red;
                r_ptr->color_ = Color::Red;
            }
            else if (r_l_c == Color::Red) // r_r_c为红 或 r_r_c 为黑 两种情况
            {
                // 进行双旋转
                ptr = doubleRotateRightForRemove(ptr);
                res = true;
            }
            else // 仅包含 r_r_c 为红，r_l_c 为黑的情况
            {
                // 进行单旋转
                ptr = SignalRotateRightForRemove(ptr);
                ChangeColorAfterRemoveRightRotate(ptr);
                res = true;
            }
        }while(false);
        return res;
    }

    bool search_for_delete_max2(Node*& ptr, const T& val, bool lock_dir, bool is_change_dir = false)
    {
        if (ptr == nullptr || ptr->right_tree_ == nullptr) return false;

        bool res = false;
        do
        {
            Node* r_ptr = ptr->right_tree_;
            if (r_ptr->color_ == Color::Red)
                break;  // 如果右子节点已经是红色, 直接返回

            if (r_ptr->right_tree_ != nullptr && r_ptr->left_tree_ != nullptr &&
                r_ptr->right_tree_->color_ == Color::Red && r_ptr->left_tree_->color_ == Color::Red)
            {
                break;
            }

            Dir dir = alg::gt(r_ptr->data_, val) ? Dir::Left : Dir::Right;
            bool change_dir = is_change_dir || (!lock_dir && dir == Dir::Left);

            if (r_ptr->right_tree_ != nullptr && r_ptr->right_tree_->color_ == Color::Red)
            {
                if (change_dir)
                {
                    r_ptr->right_tree_->color_ = Color::Black;
                    r_ptr = SignalRotateRightForRemove(r_ptr);
                    ptr->right_tree_ = r_ptr;
                    r_ptr->left_tree_->color_ = Color::Red;
                }
                break;
            }

            if (r_ptr->left_tree_ != nullptr  && r_ptr->left_tree_->color_ == Color::Red)
            {
                if (change_dir)
                    break;

                r_ptr->left_tree_->color_ = Color::Black;
                r_ptr = SignalRotateLeftForRemove(r_ptr);
                ptr->right_tree_ = r_ptr;
                r_ptr->right_tree_->color_ = Color::Red;

                break;
            }

            Node* l_ptr = ptr->left_tree_;
            assert(l_ptr != nullptr);

            Color l_l_c = get_color(l_ptr->left_tree_);
            Color l_r_c = get_color(l_ptr->right_tree_);
            // 父节点是红色，左子树只能是黑色
            if (l_l_c == Color::Black && l_r_c == Color::Black)
            {
                // 将父节点，左右子节点共同组成一个4-节点
                ptr->color_ = Color::Black;
                l_ptr->color_ = Color::Red;
                r_ptr->color_ = Color::Red;
            }
            else if (l_r_c == Color::Red) // l_l_c为红 或 l_l_c 为黑 两种情况
            {
                // 进行双旋转
                ptr = doubleRotateLeftForRemove(ptr);
                res = true;
            }
            else // 仅包含 l_l_c 为红，l_r_c 为黑的情况
            {
                // 进行单旋转
                ptr = SignalRotateLeftForRemove(ptr);
                ChangeColorAfterRemoveLeftRotate(ptr);
                res = true;
            }
        }while(false);
        return res;
    }


    // 专门用于删除时进行旋转, 不修改任何颜色，后面集中处理
    Node* SignalRotateRightForRemove(Node* gptr)
    {
        Node* pa = gptr->right_tree_;
        gptr->right_tree_ = pa->left_tree_;
        pa->left_tree_ = gptr;

        pa->parent_ = gptr->parent_;
        gptr->parent_ = pa;
        if (gptr->right_tree_ != nullptr)
            gptr->right_tree_->parent_ = gptr;

        return pa;
    }

    // 针对右单旋转之后的颜色修改
    void ChangeColorAfterRemoveRightRotate(Node* gptr)
    {
        gptr->color_ = Color::Red;
        gptr->left_tree_->left_tree_->color_ = Color::Red;
        gptr->left_tree_->color_ = Color::Black;
        gptr->right_tree_->color_ = Color::Black;
    }

    // 专门用于删除时进行旋转, 不修改任何颜色，后面集中处理
    Node* SignalRotateLeftForRemove(Node* gptr)
    {
        Node* pa = gptr->left_tree_;
        gptr->left_tree_ = pa->right_tree_;
        pa->right_tree_ = gptr;

        pa->parent_ = gptr->parent_;
        gptr->parent_ = pa;
        if (gptr->left_tree_ != nullptr)
            gptr->left_tree_->parent_ = gptr;

        return pa;
    }

    // 针对左单旋转之后的颜色修改
    void ChangeColorAfterRemoveLeftRotate(Node* gptr)
    {
        gptr->color_ = Color::Red;
        gptr->right_tree_->right_tree_->color_ = Color::Red;
        gptr->left_tree_->color_ = Color::Black;
        gptr->right_tree_->color_ = Color::Black;
    }

    // 专门用于删除时进行旋转, 包含了颜色的处理
    Node* doubleRotateRightForRemove(Node* gptr)
    {
        Node* new_p = SignalRotateLeftForRemove(gptr->right_tree_);
        gptr->right_tree_ = new_p;
        new_p = SignalRotateRightForRemove(gptr);

        ChangeColorAfterRemoveRightRotate(new_p);

        return new_p;
    }

    // 专门用于删除时进行旋转, 包含了颜色的处理
    Node* doubleRotateLeftForRemove(Node* gptr)
    {
        Node* new_p = SignalRotateRightForRemove(gptr->left_tree_);
        gptr->left_tree_ = new_p;
        new_p = SignalRotateLeftForRemove(gptr);

        ChangeColorAfterRemoveLeftRotate(new_p);

        return new_p;
    }

    Node* SignalRotateLeft(Node* gptr)
    {
        Node* pa = gptr->left_tree_;
        gptr->left_tree_ = pa->right_tree_;
        pa->right_tree_ = gptr;

        gptr->color_ = Color::Red;
        pa->color_ = Color::Black;
        
        pa->parent_ = gptr->parent_;
        gptr->parent_ = pa;
        if (gptr->left_tree_ != nullptr)
            gptr->left_tree_->parent_ = gptr;

        return pa;
    }

    Node* SignalRotateRight(Node* gptr)
    {
        Node* pa = gptr->right_tree_;
        gptr->right_tree_ = pa->left_tree_;
        pa->left_tree_ = gptr;

        gptr->color_ = Color::Red;
        pa->color_ = Color::Black;

        pa->parent_ = gptr->parent_;
        gptr->parent_ = pa;
        if (gptr->right_tree_ != nullptr)
            gptr->right_tree_->parent_ = gptr;
        
        return pa;
    }

    Node* doubleRotateLeft(Node* gptr)
    {
        Node* new_p = SignalRotateRight(gptr->left_tree_);
        gptr->left_tree_ = new_p;
        return SignalRotateLeft(gptr);
    }

    Node* doubleRotateRight(Node* gptr)
    {
        Node* new_p = SignalRotateLeft(gptr->right_tree_);
        gptr->right_tree_ = new_p;
        return SignalRotateRight(gptr);
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
    static Node* null_node_;
};

template<typename T, template <typename T1> class RbNode, typename Alloc>
typename RbTree<T, RbNode, Alloc>::Node*  RbTree<T, RbNode, Alloc>::null_node_;


}
}

#endif