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

enum class Dir
{
    Unknown = 0,
    Left,
    Right
};

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
class RbTree : protected RbTree_Base<T, RbNode, Alloc>
{
    typedef RbTree_Base<T, RbNode, Alloc> RbTreeBase;
    typedef typename RbTreeBase::Node Node;
    typedef Node*  Root;

public:
    using RbTreeBase::buy_node;
    using RbTreeBase::free_node;
    using RbTreeBase::_m_impl;

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

    // 自顶向下插入 非递归
    bool insert(const T& val)           // pass
    {
        return insert(_m_impl._root, val);
    }

    // 自顶向下删除 非递归
    bool remove(const T& val)
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

            if (alg::gt(ptr->data_, val))
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
        Node* pa = ptr;      // 作为遍历的父节点
        ptr = ptr->right_tree_;
        T val = val_param;       // 后面可能会修改待删除的val值
        Dir p_dir = Dir::Right;  // 记录遍历过程子节点位于父节点的方位
        while(ptr != nullptr)
        {
            if (alg::gt(ptr->data_, val))
            {
                left_search_for_remove(ptr, pa, p_dir);
            }
            else if (alg::le(ptr->data_, val))
            {
                right_search_for_remove(ptr, pa, p_dir);
            }
            else
            {
                Node* _ptr = ptr; // 代替ptr记录状态
                if ((ptr->left_tree_ != nullptr && ptr->right_tree_ != nullptr) ||
                     ptr->right_tree_ != nullptr)
                {
                    right_search_for_remove(ptr, pa, p_dir);
                    while(ptr->left_tree_)
                    {
                        left_search_for_remove(ptr, pa, p_dir);
                    }
                    _ptr->data_ = ptr->data_; // 替换值
                    val = ptr->data_;         // 替换待查找的val
                }
                else if (ptr->left_tree_ != nullptr)
                {
                    left_search_for_remove(ptr, pa, p_dir);
                    while(ptr->right_tree_)
                    {
                        right_search_for_remove(ptr, pa, p_dir);
                    }
                    _ptr->data_ = ptr->data_;   // 替换值
                    val = ptr->data_;
                }
                else
                {
                    if (ptr != _m_impl._root->right_tree_)
                        assert(ptr->color_ == Color::Red);
                    free_node(ptr);
                    p_dir == Dir::Right ?
                        pa->right_tree_ = nullptr : pa->left_tree_ = nullptr;
                    return true;
                }
            }
        }

        // 没有找到时返回
        return false;
    }

    void left_search_for_remove(Node*& ptr, Node*& pa, Dir& p_dir)
    {
        if (true)//(search_for_delete_min(ptr))
        {
            p_dir == Dir::Right ?
                pa->right_tree_ = ptr : pa->left_tree_ = ptr;
            // pa 和 ptr 的值依据来自于旋转后的位置
            pa = ptr->left_tree_;
            ptr = ptr->left_tree_->left_tree_;
        }
        else
        {
            // 未发生旋转
            pa =  ptr;
            ptr = ptr->left_tree_;
        }
        p_dir = Dir::Left;
    }

    void right_search_for_remove(Node*& ptr, Node*& pa, Dir& p_dir)
    {
        if (true)//(search_for_delete_max(ptr))
        {
            p_dir == Dir::Right ?
                pa->right_tree_ = ptr : pa->left_tree_ = ptr;
            pa = ptr->right_tree_;
            ptr = ptr->right_tree_->right_tree_;
        }
        else
        {
            pa = ptr;
            ptr = ptr->right_tree_;
        }
        p_dir = Dir::Right;
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
                _gptr_dir = alg::gt(_gptr->data_, gptr->data_) ? Dir::Left : Dir::Right;

            if (alg::gt(gptr->data_, ptr->data_))
            {
                // 左旋转
                if (alg::gt(pa->data_, ptr->data_)) // 单旋转
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

    // 传参为祖父节点指针
    Node* SignalRotateLeft(Node* gptr)
    {
        Node* pa = gptr->left_tree_;
        gptr->left_tree_ = pa->right_tree_;
        pa->right_tree_ = gptr;
        
        gptr->color_ = Color::Red;
        pa->color_ = Color::Black;

        return pa;
    }

    Node* SignalRotateRight(Node* gptr)
    {
        Node* pa = gptr->right_tree_;
        gptr->right_tree_ = pa->left_tree_;
        pa->left_tree_ = gptr;

        gptr->color_ = Color::Red;
        pa->color_ = Color::Black;

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

    // 专门用于删除时进行旋转, 不修改任何颜色，后面集中处理
    Node* SignalRotateRightForRemove(Node* gptr)
    {
        Node* pa = gptr->right_tree_;
        gptr->right_tree_ = pa->left_tree_;
        pa->left_tree_ = gptr;
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
                is_sort &= alg::gt(ptr->data_, ptr->left_tree_->data_);
            if (ptr->right_tree_)
                is_sort &= alg::gt(ptr->right_tree_->data_, ptr->data_);

            res.first = is_sort;
            res.second = l_info.second + (c == Color::Black ? 1 : 0);
        } while (false);

        return res;
    }

    Color get_color(Node* ptr)
    {
        if (ptr == nullptr) return Color::Black;  // 约定默认空节点为黑色

        return ptr->color_;
    }

    static int32_t _get_hight(Node* ptr)
    {
        if (ptr == nullptr) return -1;

        return alg::max(_get_hight(ptr->left_tree_), _get_hight(ptr->right_tree_)) + 1;
    }

    static void _InOrder(Node* ptr)
    {
        if (ptr)
        {
            if (ptr->left_tree_) _InOrder(ptr->left_tree_);
            stream << ptr->data_ << " ";
            if (ptr->right_tree_) _InOrder(ptr->right_tree_);
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
    static Node*  null_node_;
};

template<typename T, template <typename T1> class RbNode, typename Alloc>
typename RbTree<T, RbNode, Alloc>::Node*  RbTree<T, RbNode, Alloc>::null_node_;

}
}

#endif