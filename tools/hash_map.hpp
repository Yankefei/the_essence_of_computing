#ifndef _TOOLS_HASH_MAP_H_
#define _TOOLS_HASH_MAP_H_

#include <memory>
#include <cassert>

#include "hash_func.h"

#include "pair.hpp"

namespace tools
{

enum class KindOfEntry
{
    Empty,
    Used,
    Deleted
};

template<typename T, typename V>
struct _Entry
{
    typedef Pair<T, V>    value_type;

    _Entry(const value_type& val) : data_(val) {}
    _Entry(const T& t, const V& v) : data_(t, v) {}

    std::size_t hash_val_{0};   // 保存该data_.first的hash值用于扩展哈希时避免计算
    KindOfEntry status_{KindOfEntry::Empty};
    value_type  data_;
};

template<typename T, typename V,
        template <typename T1, typename V1> class HEntry, typename Alloc>
class HashMap_Base
{
public:
    // 申请HEntry节点
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<HEntry<T, V>> _HEntry_alloc_type;
    typedef typename std::allocator_traits<_HEntry_alloc_type>
        entry_rebind_traits;
    typedef typename entry_rebind_traits::pointer  entry_pointer;

   // 一次申请的内存数为1字节, 作为哈希数组的内存
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<uint8_t> _Byte_alloc_type;   
    typedef typename std::allocator_traits<_Byte_alloc_type>
        byte_rebind_traits;
    typedef typename byte_rebind_traits::pointer  byte_pointer;

    typedef HEntry<T, V>        Entry;

public:
    struct ByteBase_Impl : public _Byte_alloc_type {};  // 仅仅用来指导申请内存
    static ByteBase_Impl _m_byte_impl_;                 // 无状态，设置为静态变量


    struct EntryBase_Impl : public _HEntry_alloc_type {};  // 仅仅用来指导申请内存
    static EntryBase_Impl _m_entry_impl_;                  // 无状态，设置为静态变量

    HashMap_Base() {}
    ~HashMap_Base() {} // 设置虚函数，Base_Impl 会额外增加8字节内存

public:
    // 入参为需要申请的void数组的长度
    void* buy_array(size_t size)
    {
        if (size == 0) return nullptr;
        size_t total_size = size * sizeof(void*) + sizeof(size_t);
        byte_pointer b_p = byte_rebind_traits::allocate(_m_byte_impl_, total_size);
        memset(b_p, 0, total_size);
        size_t* s_p = reinterpret_cast<size_t*>(b_p);
        *s_p = size; // 数组头部放置数组的长度信息
        return static_cast<void*>(s_p + 1);
    }

    void free_array(void* array)
    {
        assert(array != nullptr);
        size_t* s_p = static_cast<size_t*>(array) - 1;
        size_t total_size = (*s_p * sizeof(void*)) + sizeof(size_t);
        byte_rebind_traits::deallocate(_m_byte_impl_,
            reinterpret_cast<byte_pointer>(s_p), total_size);
    }

    Entry* buy_entry(const Pair<T, V>& val)
    {
        entry_pointer p = entry_rebind_traits::allocate(_m_entry_impl_, 1);
        entry_rebind_traits::construct(_m_entry_impl_, p, val);
        return static_cast<Entry*>(p);
    }

    void free_entry(Entry* ptr)
    {
        entry_rebind_traits::destroy(_m_entry_impl_, ptr);
        entry_rebind_traits::deallocate(_m_entry_impl_, ptr, 1);
    }
};

template <typename T, typename V,
         template <typename T1, typename V1> class HEntry,
         typename Alloc>
typename HashMap_Base<T, V, HEntry, Alloc>::EntryBase_Impl
    HashMap_Base<T, V, HEntry, Alloc>::_m_entry_impl_;

template <typename T, typename V,
         template <typename T1, typename V1> class HEntry,
         typename Alloc>
typename HashMap_Base<T, V, HEntry, Alloc>::ByteBase_Impl
    HashMap_Base<T, V, HEntry, Alloc>::_m_byte_impl_;

/*
    容器描述：
    使用开放地址法的平方探测来处理散列冲突, 并支持扩展散列表， 其中：
    1. HashMap的表大小为 4k + 3, k为整数，保证了平方探测的逻辑。
    2. HashMap的元素大小达到表大小M的45%时, 会自动触发扩展散列表为之前的2倍，而不是达到理论上的
       50%临界点才开始扩展，保证了整体的查询效率。
    3. HashMap采用懒惰删除元素的方案，如果删除元素，则仅仅标记为释放状态, 查询时自动跳过，后面可
       被重复插入。
*/
template<typename T, typename V,
         typename H = Hash<T>,
         template <typename T1, typename V1> class HEntry = _Entry,
         typename Alloc = std::allocator<T>>
class HashMap : protected HashMap_Base<T, V, HEntry, Alloc>
{
    typedef HashMap_Base<T, V, HEntry, Alloc>    HashMapBase;
    typedef typename HashMapBase::Entry          Entry;
    typedef typename Entry::value_type           value_type;

public:
    using HashMapBase::buy_array;
    using HashMapBase::free_array;
    using HashMapBase::buy_entry;
    using HashMapBase::free_entry;

public:
    HashMap() {}
    ~HashMap() { destory(); }

private:
    size_t get_array_len()
    {
        if (array_ == nullptr) return 0;
        size_t* s_p = reinterpret_cast<size_t*>(array_) - 1;
        return *s_p;
    } 

    void destory()
    {
        if (array_ != nullptr)
        {
            for (auto index = get_array_len() - 1; index >= 0; index --)
            {
                if (array_[index] != nullptr)
                {
                    free_entry(array_[index]);
                    array_[index] = nullptr;
                }
            }
            free_array(static_cast<void*>(array_));
            array_ = nullptr;
            ele_size_ = 0;
        }
    }

private:
    std::size_t  ele_size_{0};
    Entry**      array_{nullptr};        // 使用指针数组，扩展散列表时更加高效
};


}
#endif