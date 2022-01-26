#ifndef _TOOLS_BALANCE_TREE_BASE_H_
#define _TOOLS_BALANCE_TREE_BASE_H_

#include <memory>
#include <cassert>

#include "algorithm.hpp"

namespace tools
{

/*
    在申请内存和调用构造函数的过程中, 分别使用了两个类型模板，以1字节为单位申请
    调用了BNode的构造函数
    在析构和释放内存的时候也调用对应版本的函数

    // TODO 做法还是有点飘, 待完善
*/
template<typename T,
        template <typename T1> class BNode,
        typename Alloc,
        std::size_t t_entry_size/* sizeof(Entry) */>
class BalanceTree_Base
{
public:
    // 一次申请的内存数为1字节
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<uint8_t> _Byte_alloc_type;   

    typedef typename std::allocator_traits<_Byte_alloc_type>
        byte_rebind_traits;

    typedef typename byte_rebind_traits::pointer  pointer;

    // 一次申请的内存数为 BNode大小
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<BNode<T>> _BNode_alloc_type;

    typedef typename std::allocator_traits<_BNode_alloc_type>
        rebind_traits;

    typedef BNode<T>        Node;

private:
    struct ByteBase_Impl : public _Byte_alloc_type {};  // 仅仅用来申请内存
    static ByteBase_Impl _m_byte_impl_;                 // 设置为静态变量

public:
    struct Base_Impl : public _BNode_alloc_type
    {
        Base_Impl(): _BNode_alloc_type(), _root()
        {
        }

        ~Base_Impl() {}

        Base_Impl(Base_Impl& _x) noexcept
        {
            std::swap(_root, _x._root);
        }

        Node*     _root{nullptr};
    };

    BalanceTree_Base(size_t m) : _m_size(m * t_entry_size) {}
    ~BalanceTree_Base() {} // 设置虚函数 BalanceTree_Base 会额外增加8字节内存

public:
    Base_Impl   _m_impl;
    size_t      _m_size;   // B树的阶 * sizeof(Entry)

    Node* buy_node()
    {
        static uint64_t s_node_size = sizeof(Node) + _m_size; // 实际结构体的大小设置为静态的
        pointer p = byte_rebind_traits::allocate(_m_byte_impl_, s_node_size);
        rebind_traits::construct(_m_impl, p);
        return reinterpret_cast<Node*>(p);
    }

    void free_node(Node* ptr)
    {
        rebind_traits::destroy(_m_impl, ptr);
        static uint64_t s_node_size = sizeof(Node) + _m_size; // 实际结构体的大小设置为静态的
        byte_rebind_traits::deallocate(_m_byte_impl_,
                reinterpret_cast<pointer>(ptr), s_node_size);
    }
};

template<typename T, template <typename T1> class BNode, typename Alloc, std::size_t size>
typename BalanceTree_Base<T, BNode, Alloc, size>::ByteBase_Impl
    BalanceTree_Base<T, BNode, Alloc, size>::_m_byte_impl_;


}

#endif