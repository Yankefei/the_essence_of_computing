#ifndef _TOOLS_BINARY_RB_TREE_H_
#define _TOOLS_BINARY_RB_TREE_H_

#include <memory>
#include <cassert>

#include "vector.hpp"
#include "queue.h"
#include "algorithm.hpp"
#include "pair.hpp"

#include "binary_rb_tree_base.h"

#define HAS_COLOR
#include "binary_tree_print_util.h"

namespace tools
{

namespace rb_tree_1
{

template<typename T>
struct _RbNode
{
    _RbNode() = default;
    _RbNode(const T&val) : data_(val) {}
    _RbNode(const T&val, Color c) : data_(val), color_(c) {}

    _RbNode* left_tree_{nullptr};
    _RbNode* right_tree_{nullptr};
    Color color_{Color::Black};
    T data_;
};


/*红黑树 非递归版本*/
template<typename T, template <typename T1> class RbNode = _RbNode, typename Alloc = std::allocator<T>>
class RbTree : protected RbTree_Base<T, RbNode, Alloc>, protected RbTree_Util<T, RbNode>
{
    typedef RbTree_Base<T, RbNode, Alloc> RbTreeBase;
    typedef typename RbTreeBase::Node Node;
    typedef Node*  Root;

    typedef RbTree_Util<T, RbNode> RbTreeUtil;

public:
    using RbTreeBase::buy_node;
    using RbTreeBase::free_node;
    using RbTreeBase::_m_impl;

    // 算法函数
    using RbTreeUtil::handle_before_remove;
    using RbTreeUtil::search_for_delete_min2;
    using RbTreeUtil::search_for_delete_max2;

    using RbTreeUtil::SignalRotateLeft;
    using RbTreeUtil::SignalRotateRight;
    using RbTreeUtil::doubleRotateLeft;
    using RbTreeUtil::doubleRotateRight;
    using RbTreeUtil::SignalRotateRightForRemove;
    using RbTreeUtil::ChangeColorAfterRemoveRightRotate;
    using RbTreeUtil::SignalRotateLeftForRemove;
    using RbTreeUtil::ChangeColorAfterRemoveLeftRotate;
    using RbTreeUtil::doubleRotateRightForRemove;
    using RbTreeUtil::doubleRotateLeftForRemove;
    using RbTreeUtil::get_color;
    using RbTreeUtil::_get_hight;
    using RbTreeUtil::_InOrder;
    using RbTreeUtil::_is_rb_tree;

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

    // 自顶向下插入 非递归
    bool insert(const T& val)           // pass
    {
        return insert(_m_impl._root, val);
    }

    // 自顶向下删除 非递归
    bool remove(const T& val)            // pass
    {
        bool res = remove(_m_impl._root, val);

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

    void print_tree()
    {
        draw_tree<Node>(_m_impl._root->right_tree_);
    }

private:
    // 自顶向下 非递归
    // 将向下的分解和向上的旋转放在了一起处理, 和递归程序截然不同
    bool insert(Node* root, const T& val)
    {
        Node* ptr = root->right_tree_;
        Node* pa = root;   // 父节点
        if (ptr == nullptr)
        {
            pa->right_tree_ = buy_node(val);
            return true;
        }
        Node* gptr = root; // 祖父节点
        Node* _gptr = root; // 曾祖父节点

        Dir insert_dir = Dir::Unknown;
        while(ptr != nullptr)
        {
            if (get_color(ptr->left_tree_) == Color::Red && get_color(ptr->right_tree_) == Color::Red)
            {
                // 这里是保证每一个节点到Null指针的路径上黑色节点个数相同的保证
                ptr->left_tree_->color_ = Color::Black;
                ptr->right_tree_->color_ = Color::Black;
                ptr->color_ = Color::Red;
                balance_check(ptr, pa, gptr, _gptr, val);
                
                // 还原根节点颜色, 只有在父节点是root的情况下才可能出现
                // pa在balance_check之后可能会被改变，不过这里仅仅使用值做判断
                if (pa == _m_impl._root)
                    ptr->color_ = Color::Black;
                    //_m_impl._root->right_tree_->color_ = Color::Black;
            }

            // 如果经过 balance_check之后, 理论上下面的值都可能会被修改
            // 该算法之所以正确，是在于: 经过了处理后，ptr会从原来gptr的位置继续向下遍历
            // 遍历的过程，一定不会出现需要再次 balance_check的场景，随着ptr的遍历
            // _gptr, gptr, pa 等指针的值也会慢慢全部被替换为正确的值，后面的逻辑也就保证了正确
            _gptr = gptr;  gptr = pa; pa = ptr;

            if (alg::le(val, ptr->data_))
            {
                ptr = ptr->left_tree_;
                insert_dir = Dir::Left;
            }
            else if (alg::le(ptr->data_, val))
            {
                ptr = ptr->right_tree_;
                insert_dir = Dir::Right;
            }
            else
            {
                return false;
            }
        }
        
        // ptr == nullptr
        // 默认初始插入节点颜色为 Red
        if (insert_dir == Dir::Left)
        {
            ptr = buy_node(val, Color::Red);
            pa->left_tree_ = ptr;
        }
        else
        {
            ptr = buy_node(val, Color::Red);
            pa->right_tree_ = ptr;
        }

        // 可能存在两个红色节点相连
        if (pa->color_ == Color::Red)
            balance_check(ptr, pa, gptr, _gptr, val);

        return true;
    }

    // 自顶向下，非递归
    // 改写自逻辑清晰的递归版本，算法过程与递归版本完全一致
    bool remove(Node* ptr, const T& val_param)
    {
        Node* pptr = ptr;      // 作为遍历的父节点
        ptr = ptr->right_tree_;
        T val = val_param;       // 后面可能会修改待删除的val值
        handle_before_remove(ptr, pptr, val);

        Dir dir = Dir::Right;
        Dir dir_change = Dir::Unknown; // 控制替换节点后的方向变化
        bool res = false;
        Node* old_ptr = nullptr; // 代替ptr记录状态
        while(ptr != nullptr)
        {
            if (alg::le(val, ptr->data_))
            {
                res = search_for_delete_min2(ptr, val, false);
                dir == Dir::Left ? pptr->left_tree_ = ptr : pptr->right_tree_ = ptr;
                if (res)
                {
                    pptr = ptr->left_tree_;
                    ptr = pptr->left_tree_;
                }
                else
                {
                    pptr = ptr;
                    ptr = ptr->left_tree_;
                }
                dir = Dir::Left;
            }
            else if (alg::le(ptr->data_, val))
            {
                res = search_for_delete_max2(ptr, val, false);   // 仅依据数据来转向
                dir == Dir::Left ? pptr->left_tree_ = ptr : pptr->right_tree_ = ptr;
                if (res)
                {
                    pptr = ptr->right_tree_;  // 更新成ptr最新的父指针
                    ptr = pptr->right_tree_;
                }
                else
                {
                    pptr = ptr;
                    ptr = ptr->right_tree_;
                }
                dir = Dir::Right;
            }
            else
            {
                old_ptr = ptr;             // 重置
                dir_change = Dir::Unknown; // 重置
                if ((ptr->left_tree_ != nullptr && ptr->right_tree_ != nullptr) ||
                    ptr->right_tree_ != nullptr)
                {
                    // 这里处理之后，会发生遍历线路的拐弯，需要特殊处理
                    res = search_for_delete_max2(ptr, val, true, true); // 强制转向
                    dir == Dir::Left ? pptr->left_tree_ = ptr : pptr->right_tree_ = ptr;
                    if (res)
                    {
                        pptr = ptr->right_tree_;
                        ptr = pptr->right_tree_;
                    }
                    else
                    {
                        pptr = ptr;
                        ptr = ptr->right_tree_;
                    }

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
                            pptr = ptr->left_tree_;  // 更新父指针
                            ptr = pptr->left_tree_;
                        }
                        else
                        {
                            if (dir_change == Dir::Unknown)
                                dir_change = Dir::Left;
                            pptr = ptr;
                            ptr = ptr->left_tree_;
                        }
                    }
                    old_ptr->data_ = ptr->data_;
                    val = ptr->data_;
                    dir_change == Dir::Unknown ? dir = Dir::Right : dir = Dir::Left;
                }
                else if (ptr->left_tree_ != nullptr)
                {
                    res = search_for_delete_min2(ptr, val, true, true);
                    dir == Dir::Left ? pptr->left_tree_ = ptr : pptr->right_tree_ = ptr;
                    if (res)
                    {
                        pptr = ptr->left_tree_;  // 更新成ptr最新的父指针
                        ptr = pptr->left_tree_;
                    }
                    else
                    {
                        pptr = ptr;
                        ptr = ptr->left_tree_;
                    }

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
                            pptr = ptr->right_tree_;
                            ptr = pptr->right_tree_;
                        }
                        else
                        {
                            if (dir_change == Dir::Unknown)
                                dir_change = Dir::Right;
                            pptr = ptr;
                            ptr = ptr->right_tree_;
                        }
                    }
                    old_ptr->data_ = ptr->data_;
                    val = ptr->data_;
                    dir_change == Dir::Unknown ? dir = Dir::Left : dir = Dir::Right;
                }
                else
                {
                    if (ptr != _m_impl._root->right_tree_)
                        assert(ptr->color_ == Color::Red);
                    free_node(ptr);
                    ptr = nullptr;
                    dir == Dir::Left ? pptr->left_tree_ = ptr : pptr->right_tree_ = ptr;
                    return true;
                }
            }
        }

        // 没有找到，返回false
        return false;
    }

    // check p_ptr和 p_pa 是否同时为red节点
    // 将所有的单旋转和双旋转同时进行考虑
    void balance_check(Node* ptr, Node* pa, Node* gptr, Node* _gptr, const T& val)
    {
        if (ptr->color_ == Color::Red && pa->color_ == Color::Red)
        {
            assert(gptr->color_ == Color::Black);
            // 如果曾祖父节点是根节点，那么默认是右边
            // 该判断位于ptr和pa节点为红色的情况下，在程序一开始的过程中，也就是 gptr和_gptr都是
            // 根节点， 是不可能进入到这里的。
            Dir _gptr_dir = Dir::Right;
            if (_gptr != _m_impl._root)
                _gptr_dir = alg::le(gptr->data_, _gptr->data_) ? Dir::Left : Dir::Right;

            if (alg::le(ptr->data_, gptr->data_))
            {
                // 左旋转
                if (alg::le(ptr->data_, pa->data_)) // 单旋转
                {
                    if (_gptr_dir == Dir::Left)
                        _gptr->left_tree_ = SignalRotateLeft(gptr);
                    else
                        _gptr->right_tree_ = SignalRotateLeft(gptr);
                }
                else // 双旋转
                {
                    if (_gptr_dir == Dir::Left)
                        _gptr->left_tree_ = doubleRotateLeft(gptr);
                    else
                        _gptr->right_tree_ = doubleRotateLeft(gptr);
                }
            }
            else
            {
                // 右旋转
                if (alg::le(pa->data_, ptr->data_)) // 单旋转
                {
                    if (_gptr_dir == Dir::Left)
                        _gptr->left_tree_ = SignalRotateRight(gptr);
                    else
                        _gptr->right_tree_ = SignalRotateRight(gptr);
                }
                else  // 双旋转
                {
                    if (_gptr_dir == Dir::Left)
                        _gptr->left_tree_ = doubleRotateRight(gptr);
                    else
                        _gptr->right_tree_ = doubleRotateRight(gptr);
                }
            }
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
    /*static*/ Node*  null_node_{nullptr};
};

// template<typename T, template <typename T1> class RbNode, typename Alloc>
// typename RbTree<T, RbNode, Alloc>::Node*  RbTree<T, RbNode, Alloc>::null_node_;

}
}

#endif