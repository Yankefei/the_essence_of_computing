#ifndef _TOOLS_LOCK_FREE_CONTAINER_QUEUE_BASE_H_
#define _TOOLS_LOCK_FREE_CONTAINER_QUEUE_BASE_H_

#include <memory>
#include <cassert>
#include <cstring>

#include "general.h"

namespace tools
{

template<typename T, typename Alloc = std::allocator<T>>
class QueueBase
{
    // 一次申请的内存数为1字节
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<uint8_t> _Byte_alloc_type;   
    typedef typename std::allocator_traits<_Byte_alloc_type>
        byte_rebind_traits;
    typedef typename byte_rebind_traits::pointer  byte_pointer;

    // 一次申请的内存数为 T大小
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<T> _T_alloc_type;
    typedef typename std::allocator_traits<_T_alloc_type>
        t_rebind_traits;
    typedef typename t_rebind_traits::pointer t_pointer;

    struct ByteBase_Impl : public _Byte_alloc_type {};  // 仅仅用来指导申请内存
    static ByteBase_Impl _m_byte_impl_;                 // 无状态，设置为静态变量

    struct TBase_Impl : public _T_alloc_type {};  // 仅仅用来指导申请内存
    static TBase_Impl _m_t_impl_;                 // 无状态，设置为静态变量

public:
    // 入参为需要申请的数组长度
    T* buy_array(uint32_t size)
    {
        if (size == 0) return nullptr;
        size_t total_size = size * sizeof(T) + 4;
        byte_pointer b_p = byte_rebind_traits::allocate(_m_byte_impl_, total_size);
        memset(b_p, 0, total_size);
        uint32_t* s_p = reinterpret_cast<uint32_t*>(b_p);
        *s_p = size; // 数组头部放置数组的长度信息

        // 构造T类型数据
        b_p += 4;
        for (int i = 0; i < size; i++)
        {
            t_rebind_traits::construct(_m_t_impl_, b_p + i * sizeof(T));
        }

        return reinterpret_cast<T*>(s_p + 1);
    }

    void free_array(T* array)
    {
        assert(array != nullptr);
        uint32_t* s_p = reinterpret_cast<uint32_t*>(array) - 1;
        uint32_t array_len = *s_p;
        for (int i = 0; i < array_len; i++)
        {
            t_rebind_traits::destroy(_m_t_impl_, array + i);
        }
        size_t total_size = array_len * sizeof(T) + 4;
        byte_rebind_traits::deallocate(_m_byte_impl_,
            reinterpret_cast<byte_pointer>(s_p), total_size);
    }
};

template <typename T, typename Alloc>
typename QueueBase<T, Alloc>::ByteBase_Impl QueueBase<T, Alloc>::_m_byte_impl_;

template <typename T, typename Alloc>
typename QueueBase<T, Alloc>::TBase_Impl QueueBase<T, Alloc>::_m_t_impl_;



}

#endif