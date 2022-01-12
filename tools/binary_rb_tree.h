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


template<typename T>
struct _RbNode
{
    _RbNode() = default;
    _RbNode(const T&val) : data_(val) {}

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
        return insert(&_m_impl._root, val);
    }

    // 自顶向下删除
    bool remove(const T& val)
    {

    }


    bool is_rb_tree()
    {
        if (_m_impl._root == nullptr) return true;
        
        Node* read_root = _m_impl._root->right_tree_;
        if (read_root == nullptr) return true;

        if (read_root->color_ == Color::Red) return false;

        return _is_rb_tree(read_root).first;
    }

    void in_order()
    {
        if (_m_impl._root != nullptr)
            in_order(_m_impl._root->right_tree_);
    }

    void print_tree()
    {
        if (_m_impl._root != nullptr)
            draw_tree<Node>(_m_impl._root->right_tree_);
    }

private:
    bool insert(Node**, const T& val)
    {
        
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

            res.first = true;
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