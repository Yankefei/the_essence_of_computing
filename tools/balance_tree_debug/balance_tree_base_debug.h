#ifndef _TOOLS_BALANCE_TREE_BASE_DEBUG_H_
#define _TOOLS_BALANCE_TREE_BASE_DEBUG_H_

#include <memory>
#include <cassert>
#include <cstring> // memset

#include "algorithm.hpp"

namespace tools
{

/*
    申请BNode内存的时候，额外申请一块单独的内存给array_变量，作为Entry数组,
    该内存布局版本为debug版本, 主要便于 gdb 调试
*/
template<typename T,
        typename Alloc,
        template <typename T1> class BNode,
        template <typename T2> class BEntry>
class BalanceTree_Base
{
public:
    // 一次申请的内存数为1字节
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<uint8_t> _Byte_alloc_type;   
    typedef typename std::allocator_traits<_Byte_alloc_type>
        byte_rebind_traits;
    typedef typename byte_rebind_traits::pointer  byte_pointer;

    // 一次申请的内存数为 BNode大小
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<BNode<T>> _BNode_alloc_type;
    typedef typename std::allocator_traits<_BNode_alloc_type>
        node_rebind_traits;
    typedef typename node_rebind_traits::pointer node_pointer;

    // 一次申请的内存数为 BEntry大小
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<BEntry<T>> _Entry_alloc_type;
    typedef typename std::allocator_traits<_Entry_alloc_type>
        entry_rebind_traits;
    typedef typename entry_rebind_traits::pointer entry_pointer;

    typedef BNode<T>        Node;
    typedef BEntry<T>       Entry; 

private:
    struct ByteBase_Impl : public _Byte_alloc_type {};  // 仅仅用来指导申请内存
    static ByteBase_Impl _m_byte_impl_;                 // 无状态，设置为静态变量

    struct EntryBase_Impl : public _Entry_alloc_type {};
    static EntryBase_Impl _m_entry_impl_;                // 无状态，设置为静态变量

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

    BalanceTree_Base(size_t m) : _m(m), _array_size(m * sizeof(Entry)) {}
    ~BalanceTree_Base() {} // 设置虚函数 BalanceTree_Base 会额外增加8字节内存

public:
    Base_Impl   _m_impl;
    size_t      _m;
    size_t      _array_size;   // entry指针数组所需要申请内存的大小

    // 入参为需要申请的int32_t数组的长度
    int32_t* buy_array(size_t size)
    {
        if (size == 0) return nullptr;
        size_t total_size = (size + 1) * sizeof(int32_t);
        byte_pointer b_p = byte_rebind_traits::allocate(_m_byte_impl_, total_size);
        memset(b_p, 0, total_size);
        int32_t* s_p = reinterpret_cast<int32_t*>(b_p);
        *s_p = size; // 数组头部放置数组的长度信息
        return static_cast<int32_t*>(s_p + 1);
    }

    void free_array(int32_t* array)
    {
        assert(array != nullptr);
        int32_t* s_p = array - 1;
        size_t total_size = (*s_p + 1) * sizeof(int32_t);
        byte_rebind_traits::deallocate(_m_byte_impl_,
            reinterpret_cast<byte_pointer>(s_p), total_size);
    }

    Node* buy_node()
    {
        node_pointer p = node_rebind_traits::allocate(_m_impl, 1);
        node_rebind_traits::construct(_m_impl, p);
        Node* ptr = static_cast<Node*>(p);
        // 使用字节为单位的方式申请内存
        byte_pointer b_p = byte_rebind_traits::allocate(_m_byte_impl_, _array_size);
        memset(b_p, 0, _array_size);
        for (int i = 0; i < _m; i++)
        {
            entry_rebind_traits::construct(_m_entry_impl_, b_p + i * sizeof(Entry));
        }
        ptr->array_ = reinterpret_cast<Entry*>(b_p);
        return ptr;
    }

    void free_node(Node* ptr)
    {
        Entry* array_ptr = ptr->array_;
        for (int i = 0; i < _m; i++)
        {
            entry_rebind_traits::destroy(_m_entry_impl_, array_ptr + i * sizeof(Entry));
        }
        byte_rebind_traits::deallocate(_m_byte_impl_,
            reinterpret_cast<byte_pointer>(ptr->array_), _array_size);
        node_rebind_traits::destroy(_m_impl, ptr);
        node_rebind_traits::deallocate(_m_impl, ptr, 1);
    }

    // Entry* buy_entry(const T& val)
    // {
    //     entry_pointer p = entry_rebind_traits::allocate(_m_entry_impl_, 1);
    //     entry_rebind_traits::construct(_m_entry_impl_, p, val);
    //     return static_cast<Entry*>(p);
    // }

    // void free_entry(Entry* ptr)
    // {
    //     entry_rebind_traits::destroy(_m_entry_impl_, ptr);
    //     entry_rebind_traits::deallocate(_m_entry_impl_, ptr, 1);
    // }
};

template <typename T,
         typename Alloc,
         template <typename T1> class BNode,
         template <typename T2> class BEntry>
typename BalanceTree_Base<T, Alloc, BNode, BEntry>::ByteBase_Impl
    BalanceTree_Base<T, Alloc, BNode, BEntry>::_m_byte_impl_;

template <typename T,
         typename Alloc,
         template <typename T1> class BNode,
         template <typename T2> class BEntry>
typename BalanceTree_Base<T, Alloc, BNode, BEntry>::EntryBase_Impl
    BalanceTree_Base<T, Alloc, BNode, BEntry>::_m_entry_impl_;

}

#endif