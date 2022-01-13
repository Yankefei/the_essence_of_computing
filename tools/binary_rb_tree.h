#ifndef _TOOLS_BINARY_RB_TREE_H_
#define _TOOLS_BINARY_RB_TREE_H_

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
    _RbNode(const T&val, Dir dir) : data_(val), color_(dir) {}

    _RbNode* left_tree_{nullptr};
    _RbNode* right_tree_{nullptr};
    Color color_{Color::Black};
    T data_;
};


template<typename T, typename Alloc>
class RbTree_Base
{
public:
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<_RbNode<T>> _RbNode_alloc_type;

    typedef typename std::allocator_traits<_RbNode_alloc_type>
        rebind_traits;

    typedef typename rebind_traits::pointer  pointer;

    typedef _RbNode<T>        Node;

public:
    struct Base_Impl : public _RbNode_alloc_type
    {
        Base_Impl(): _RbNode_alloc_type(), _root()
        {
        }

        ~Base_Impl() {}

        Base_Impl(Base_Impl& _x) noexcept
        {
            std::swap(_root, _x._root);
        }

        Node*     _root{nullptr};
    };

    RbTree_Base() {}
    ~RbTree_Base() {} // 设置虚函数，Base_Impl 会额外增加8字节内存

public:
    Base_Impl   _m_impl;

    Node* buy_node(const T& val)
    {
        pointer p = rebind_traits::allocate(_m_impl, 1);
        rebind_traits::construct(_m_impl, p, val);
        return static_cast<Node*>(p);
    }

    Node* buy_node(const T& val, Color c)
    {
        Node* ptr = buy_node(val);
        ptr->color_ = c;
        return ptr;
    }

    void free_node(Node* ptr)
    {
        rebind_traits::destroy(_m_impl, ptr);
        rebind_traits::deallocate(_m_impl, ptr, 1);
    }
};

/*红黑树*/
template<typename T, typename Alloc = std::allocator<T>>
class RbTree : protected RbTree_Base<T, Alloc>
{
    typedef typename RbTree_Base<T, Alloc>::Node Node;
    typedef Node*  Root;

public:
    using RbTree_Base<T, Alloc>::buy_node;
    using RbTree_Base<T, Alloc>::free_node;
    using RbTree_Base<T, Alloc>::_m_impl;

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

    // 自顶向下插入
    bool insert(const T& val)
    {
        return insert(_m_impl._root, val);
    }

    // 自顶向下删除
    bool remove(const T& val)
    {
        return remove(_m_impl._root, val);
    }

    bool is_rb_tree()
    {   
        Node* read_root = _m_impl._root->right_tree_;
        if (read_root == nullptr) return true;

        if (read_root->color_ == Color::Red) return false;

        return _is_rb_tree(read_root).first;
    }

    void in_order()
    {
        in_order(_m_impl._root->right_tree_);
    }

    void print_tree()
    {
        draw_tree<Node>(_m_impl._root->right_tree_);
    }

private:
    // 自顶向下 非递归
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
                    _m_impl._root->right_tree_->color_ = Color::Black;
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

    bool remove(Node* ptr, const T& val)
    {

    }

    // check p_ptr和 p_pa 是否同时为red节点
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

template<typename T, typename Alloc>
typename RbTree<T, Alloc>::Node*  RbTree<T, Alloc>::null_node_;

}
}

#endif