#ifndef _TOOLS_BINARY_SORT_TREE_BASE_H_
#define _TOOLS_BINARY_SORT_TREE_BASE_H_

#include <memory>
#include <cassert>

namespace tools
{

template<typename T, template <typename T1> class SNode, typename Alloc>
class BsTree_Base
{
public:
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<SNode<T>> _SNode_alloc_type;

    typedef typename std::allocator_traits<_SNode_alloc_type>
        rebind_traits;

    typedef typename rebind_traits::pointer  pointer;

    typedef SNode<T>        Node;

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

}

#endif