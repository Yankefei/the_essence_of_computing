#ifndef _TOOLS_BINARY_THREAD_TREE_H_
#define _TOOLS_BINARY_THREAD_TREE_H_

#include <memory>
#include <cassert>

namespace tools
{

enum class PointerTag
{
    LINK = 0,
    THREAD = 1
};

template<typename T = char>
struct _TNode
{
    _TNode() = default;
    _TNode(const T&val) : data_(val) {}

    _TNode* left_tree_{nullptr};
    _TNode* right_tree_{nullptr};
    T data_;
    PointerTag l_tag;
    PointerTag r_tag;
};

/*线索二叉树*/
template<typename T, typename Alloc>
class BtTree_Base
{
public:
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<_TNode<T>> _TNode_alloc_type;

    typedef typename std::allocator_traits<_TNode_alloc_type>
        rebind_traits;

    typedef typename rebind_traits::pointer  pointer;

    typedef _TNode<T>*        Node;

public:
    struct Base_Impl : public _TNode_alloc_type
    {
        Base_Impl(): _TNode_alloc_type(), _root(), _end()
        {
        }

        ~Base_Impl() {}

        Base_Impl(Base_Impl& _x) noexcept
        {
            std::swap(_root, _x._root);
            std::swap(_end, _x._end);
        }

        Node     _root{nullptr};
        T        _end;
    };

    BtTree_Base() {}
    ~BtTree_Base() {} // 设置虚函数，Base_Impl 会额外增加8字节内存

public:
    Base_Impl   _m_impl;

    Node _buy_node(const T& val)
    {
        pointer p = rebind_traits::allocate(_m_impl, 1);
        rebind_traits::construct(_m_impl, p, val);
        return static_cast<Node>(p);
    }

    void _free_node(Node ptr)
    {
        rebind_traits::destroy(_m_impl, ptr);
        rebind_traits::deallocate(_m_impl, ptr, 1);
    }
};

template<typename T = char, typename Alloc = std::allocator<T>>
class BtTree : protected BtTree_Base<T, Alloc>
{
public:
    typedef _TNode<T>*        Node;
    typedef Node              Root;

protected:
    using BtTree_Base<T, Alloc>::_buy_node;
    using BtTree_Base<T, Alloc>::_free_node;

public:
    BtTree() {}
    ~BtTree() = default;

    Node new_node(T val)
    {
        return _buy_node(val);
    }

    void free_node(Node ptr)
    {
        _free_node(ptr);
    }

private:

};


}

#endif