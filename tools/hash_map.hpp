#ifndef _TOOLS_HASH_MAP_H_
#define _TOOLS_HASH_MAP_H_

#include <memory>
#include <cassert>
#include <cmath>
#include <cstring>

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
    _Entry(T&& t, V&& v) : data_(std::forward<T>(t), std::forward<V>(v)) {}
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

    Entry* buy_entry(T&& v, V&& val)
    {
        entry_pointer p = entry_rebind_traits::allocate(_m_entry_impl_, 1);
        entry_rebind_traits::construct(_m_entry_impl_,
                                       std::forward<T>(v), std::forward<V>(val));
        return static_cast<Entry*>(p);
    }

    Entry* buy_entry(const T& v, const V& val)
    {
        entry_pointer p = entry_rebind_traits::allocate(_m_entry_impl_, 1);
        entry_rebind_traits::construct(_m_entry_impl_, p, v, val);
        return static_cast<Entry*>(p);
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
    使用开放地址法的平方探测来处理散列冲突, 并支持自动扩展和紧缩散列表， 其中：
    1. HashMap的表大小为 4k + 3, k为整数，保证了平方探测的遍历逻辑。
    2. HashMap的实际元素大小达到表大小M的50%时, 会自动触发扩展哈希表的逻辑。
    3. HashMap采用懒惰删除元素的方案，如果删除元素，则仅仅标记为释放状态, 查询时自动跳过，后面可
       被重复插入。如果在频繁插入，删除的极端过程中，表内存被删除的节点数量过多，也会影响整体性能，
       需要进行额外的处理。
    4. 当有效元素数据小于表大小的1/8时，紧缩表

    所以综上，该容器重新Hash的操作的前提是：
    1. 当表中有效和被删除的节点数量之和达到50%。进行重新哈希操作，如果其中被删除的节点数量超过有效
       节点的数量一半时, 则仅重新哈希，不扩展哈希表。在重新Hash的时候会清理掉这些被删除的节点。在
       插入时才会触发。
    2. 当表的有效数据小于表大小的1/8时，重新Hash。在删除数据时才会触发。

    平方探测的正确性依据有下面两个被证明的结论：
    1. 当表的长度m为素数且表的装满程度不超过0.5时，新的元素一定能够插入，而且任何一个位置不会被探查
       2次。
    2. 表大小为4k+3的素数, 且使用的平方冲突解决方案为 F(i)= +-(1^2), 那么整个表均可被探测到。
            For primes of the form 4k + 3, we can construct a "quadratic" sequence which
        examines the whole table in the first d steps。
                -- from http://vlado.fmf.uni-lj.si/vlado/papers/QuadHash.pdf

    所以，该容器的非常适合插入之后，高频的查询，且删除元素较少的场景。
*/
template<typename T, typename V,
         template <typename T1> typename HashFunc = Hash,
         template <typename T2, typename V1> class HEntry = _Entry,
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

    bool insert(const T& key, const V& val)
    {
        Entry* entry = buy_entry(key, val);
        return insert(entry);
    }

    bool emplace(T&& key, V&& val)
    {
        Entry* entry = buy_entry(std::forward<T>(key), std::forward<V>(val));
        return insert(entry);
    }

    bool find(const T& key)
    {
        size_t hash_val = HashFunc<T>()(key);
        size_t index = find(key, hash_val);
        if (array_[index] != nullptr && array_[index]->status_ == KindOfEntry::Used)
            return true;
        
        return false;
    }

    bool remove(const T& key)
    {
        size_t hash_val = HashFunc<T>()(key);
        size_t index = find(key, hash_val);
        if (array_[index] != nullptr && array_[index]->status_ == KindOfEntry::Used)
        {
            array_[index]->status_ = KindOfEntry::Deleted;
            delete_ele_size_ ++;
            ele_size_ --;
            if (k_num_ > 5 && ele_size_ << 3 < array_len_) // 小于表的1/8, 缩表一半
            {
                size_t new_len = retrench_array_len();
                handle_rehash(new_len);
            }
            return true;
        }

        return false;
    }

    size_t size()
    {
        return ele_size_;
    }

    bool empty()
    {
        return size() == 0;
    }

private:
    // 入参为hash的返回值, 返回数据实际的序号, 或者应该插入的序号
    // 探测的方式为 f = +- i^2
    size_t find(const T& key, size_t hash_val)
    {
        size_t index = hash_val % array_len_;
        size_t index2 = index; // 反向遍历的位置记录

        bool odd = false;   // 用于+- 探测的转换
        size_t k = 0; // 平方探测的次数
        Entry* entry_ptr = array_[index];
        if (entry_ptr == nullptr) return index;

        // 这里的循环能够跳出源于已经证明的结论
        while((entry_ptr->status_ == KindOfEntry::Used && alg::neq(entry_ptr->data_.first, key))
             || entry_ptr->status_ == KindOfEntry::Deleted)
        {
            if (!odd) // 正向
            {
                k++;
                index = (index + 2*k -1) % array_len_; // 根据递推公式得到的探测距离， 1,4,9,16...
                odd = true;
                entry_ptr = array_[index];
            }
            else // 反向
            {
                index2 = index2 - 2*k + 1;         // 根据递推公式得到的探测距离， -1,-4,-9,-16...
                if (index2 < 0) index2 += array_len_;
                odd = false;
                entry_ptr = array_[index2];
            }

            if (entry_ptr == nullptr) break;
            odd = !odd;
        }
        return odd ? index2 : index;
    }

    bool insert(Entry* entry)
    {
        // 进行哈希重置
        if (ele_size_ == 0)
        {
            size_t new_len = expand_array_len();
            handle_rehash(new_len);
        }
        else if (ele_size_ + delete_ele_size_ + 1 >= (array_len_ * 50 / 100))
        {
            // 当被删除节点没有超过有效节点数量一半时，才扩展表大小
            if (ele_size_ - delete_ele_size_ >= delete_ele_size_)
            {
                size_t new_len = expand_array_len();
                handle_rehash(new_len);
            }
            else
            {
                handle_rehash(array_len_);
            }
        }

        entry->hash_val_ = HashFunc<T>()(entry->data_.first);
        entry->status_ = KindOfEntry::Used;
        size_t pos = find(entry->data_.first, entry->hash_val_);
        array_[pos] = entry;
        ele_size_ ++;
        return true;
    }

    void handle_rehash(size_t new_len)
    {
        assert(new_len > ele_size_);
        Entry** new_array = reinterpret_cast<Entry**>(buy_array(new_len));
        Entry* ptr = nullptr;
        for (size_t i = 0; i < array_len_; i++)
        {
            ptr = array_[i];
            if (ptr != nullptr)
            {
                if (ptr->status_ == KindOfEntry::Used)
                {
                    new_array[ptr->hash_val_ % new_len] = ptr;
                }
                else // Deleted
                {
                    assert(ptr->status_ == KindOfEntry::Deleted);
                    free_entry(ptr);
                }
            }
        }
        delete_ele_size_ = 0;
        if (array_ != nullptr) free_array(array_);
        array_ = new_array;
        array_len_ = new_len;
    }

    // 扩增表大小
    // 获取下一个满足 4k + 3 且为现有元素数量至少4倍的素数
    size_t expand_array_len()
    {
        size_t new_array_len = 0;
        if (array_len_ == 0)
        {
            new_array_len = 4 * k_num_ + 3;
        }
        else
        {
            // size_t next_min_num = array_len_ * 45 * 4 / 100; // 用于测试
            // assert(ele_size_ > (array_len_ * 45 / 100));        // 扩展条件
            size_t next_min_num = ele_size_ * 4;
            do
            {
                new_array_len = 4 * ++k_num_ + 3;
            }while(new_array_len < next_min_num || !is_prime_num(new_array_len));
        }
        /*
        如果表元素数量达到45%的容量就扩展表大小为至少是现有元素数量数目的2倍，那么下面是前15次表
        大小的数量变化情况:

        expand i: 1, k: 5, array_len_: 23
        expand i: 2, k: 10, array_len_: 43
        expand i: 3, k: 19, array_len_: 79
        expand i: 4, k: 37, array_len_: 151
        expand i: 5, k: 67, array_len_: 271
        expand i: 6, k: 121, array_len_: 487
        expand i: 7, k: 220, array_len_: 883
        expand i: 8, k: 401, array_len_: 1607
        expand i: 9, k: 725, array_len_: 2903
        expand i: 10, k: 1306, array_len_: 5227
        expand i: 11, k: 2354, array_len_: 9419
        expand i: 12, k: 4240, array_len_: 16963
        expand i: 13, k: 7634, array_len_: 30539
        expand i: 14, k: 13744, array_len_: 54979
        expand i: 15, k: 24740, array_len_: 98963
        */
        // stream << "k: " << k_num_ << ", array_len_: " << array_len_ << std::endl;
        return new_array_len;
    }

    // 紧缩表大小
    // 获取下一个满足 4k + 3 且为小于等于有效元素数的4倍，(现有表大小的一半及以下)
    size_t retrench_array_len()
    {
        size_t new_array_len = 0;
        size_t next_max_num = ele_size_ * 4;
        do
        {
            new_array_len = 4 * --k_num_ + 3;
            if (k_num_ == 5) break;
        }while(new_array_len > next_max_num || !is_prime_num(new_array_len));
        return new_array_len;
    }

    bool is_prime_num(std::size_t num)
    {
        if (num % 2 == 0) return false;
        // 从奇数中筛选
        size_t square_num = ::sqrt(num);
        for (size_t i = 3; i <= square_num; i += 2)
        {
            if ( num % i  == 0) return false;
        }
        return true;
    }

    void destory()
    {
        if (array_ != nullptr)
        {
            for (auto index = 0; index < array_len_; index ++)
            {
                if (array_[index] != nullptr)
                {
                    free_entry(array_[index]);
                }
            }
            free_array(static_cast<void*>(array_));
            array_ = nullptr;
            ele_size_ = 0;
            delete_ele_size_ = 0;
            array_len_ = 0;
            k_num_ = 5;
        }
    }

private:
    std::size_t  ele_size_{0};           // 有效元素的数量
    std::size_t  delete_ele_size_{0};    // 已经被删除的元素数量
    Entry**      array_{nullptr};        // 使用指针数组，扩展散列表时更加高效

    std::size_t  k_num_{5};              // 表大小计算所使用的倍数k, 初始化为5
    std::size_t  array_len_{0};          // 数组长度 满足 4k + 3 的素数
};


}
#endif