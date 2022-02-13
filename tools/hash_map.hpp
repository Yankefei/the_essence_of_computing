#ifndef _TOOLS_HASH_MAP_H_
#define _TOOLS_HASH_MAP_H_

#include <memory>
#include <cassert>
#include <cmath>
#include <cstring>

#include "hash_func.h"
#include "binary_sort_tree3.h"

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

    std::size_t hash_val_{0};                // 保存该data_.first的hash值用于扩展哈希时避免计算
    void*       iter_ptr_{nullptr};          // 保存迭代器索引结构的指针, 便于快速查找
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
    5. 使用一棵排序二叉树（非平衡版）保存数据节点在数组中的位置信息，配合迭代器遍历高效容器。且不
       影响查询效率。

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

    typedef typename BsTree<size_t>::Node        IndexNode;

    struct Iter
    {
        friend class HashMap<T, V>;
        Iter() = default;
        Iter(HashMap<T, V>* p, IndexNode* ptr)
            : parent_(p), node_ptr_(ptr)
        {}

        Iter(const Iter& it)
            : parent_(it.parent_), node_ptr_(it.node_ptr_)
        {}

        Iter& operator=(const Iter& rhs)
        {
            node_ptr_ = rhs.node_ptr_;
            parent_ = rhs.parent_;
            return *this;
        }

        /*const*/ value_type* operator->()
        {
            return &**this;
        }

        /*const*/ value_type& operator*()
        {
            assert(parent_ != nullptr);
            // if (parent_ == nullptr)
            //     return value_type();
            return parent_->array_[node_ptr_->data_]->data_;
        }

        Iter& operator++()
        {
            if (parent_ == nullptr)
                return *this;
            node_ptr_ = parent_->next_index(node_ptr_);
            return *this;
        }

        Iter operator++(int)
        {
            Iter temp = *this;
            ++*this;
            return temp;
        }

        bool operator==(const Iter& rhs)
        {
            return node_ptr_ == rhs.node_ptr_ && parent_ == rhs.parent_;
        }

        bool operator!=(const Iter& rhs)
        {
            return !(*this == rhs);
        }

    private:
        HashMap<T, V>*  parent_{nullptr};
        IndexNode*      node_ptr_{nullptr}; // 使用地址索引节点的指针作为迭代器的成员
    };

    friend class Iter;

public:
    using HashMapBase::buy_array;
    using HashMapBase::free_array;
    using HashMapBase::buy_entry;
    using HashMapBase::free_entry;

public:
    HashMap() {}
    ~HashMap() { destory(); }

    HashMap(const HashMap& rhs)
    {

    }

    HashMap& operator=(const HashMap& rhs)
    {
        return *this;
    }

    HashMap(HashMap&& rhs)
    {

    }

    HashMap& operator=(HashMap&& rhs)
    {
        return *this;
    }

    bool insert(const T& key, const V& val)
    {
        size_t hash_val = HashFunc<T>()(key);
        auto index_pair = find(key, hash_val);
        if (index_pair.second != nullptr)
            return false;

        Entry* entry = buy_entry(key, val);
        entry->hash_val_ = hash_val;
        entry->status_ = KindOfEntry::Used;
        return insert(entry, index_pair.first);
    }

    bool emplace(T&& key, V&& val)
    {
        size_t hash_val = HashFunc<T>()(key);
        auto index_pair = find(key, hash_val);
        if (index_pair.second != nullptr)
            return false;

        Entry* entry = buy_entry(std::forward<T>(key), std::forward<V>(val));
        entry->hash_val_ = hash_val;
        entry->status_ = KindOfEntry::Used;
        return insert(entry, index_pair.first);
    }

    Iter find(const T& key)
    {
        size_t hash_val = HashFunc<T>()(key);
        auto index_pair = find(key, hash_val);
        if (index_pair.second != nullptr)
        {
            assert(index_pair.second->status_ == KindOfEntry::Used);
            return Iter(this, static_cast<IndexNode*>(index_pair.second->iter_ptr_));
        }

        return Iter(this, nullptr);
    }

    size_t erase(const T& key)
    {
        size_t hash_val = HashFunc<T>()(key);
        auto index_pair = find(key, hash_val);
        if (index_pair.second != nullptr)
        {
            remove(index_pair.second);
            return 1;
        }

        return 0;
    }

    Iter erase(Iter it)
    {
        do
        {
            if (it == end())
                break;

            Entry* entry = array_[it.node_ptr_->data_];
            if (entry == nullptr)
                break;

            Iter re = ++it;
            auto new_index = remove(entry); // 重置哈希表会将迭代器失效
            if (new_index == nullptr)
                return re;

            return Iter(this, new_index);
        }while(false);
        return Iter(this, nullptr);
    }

    V& operator[](const T& key)
    {
        size_t hash_val = HashFunc<T>()(key);
        auto index_pair = find(key, hash_val);
        if (index_pair.second != nullptr)
            return index_pair.second->data_.first;

        Entry* entry = buy_entry(key, V());
        entry->hash_val_ = hash_val;
        entry->status_ = KindOfEntry::Used;
        assert(insert(entry, index_pair.first) == true);
        return entry->data_.first;
    }

    size_t size()
    {
        return ele_size_;
    }

    bool empty()
    {
        return size() == 0;
    }

    /*
        迭代器在插入，删除元素后都可能会失效
    */
    Iter begin()
    {
        IndexNode* ptr = index_tree_.first();

        while(ptr != nullptr)
        {
            assert(array_[ptr->data_] != nullptr); // 没有删除，一定是有效的
            if (array_[ptr->data_]->status_ == KindOfEntry::Deleted)
                ptr = BsTree<size_t>::next(ptr);
            else
                break;
        }
        return Iter(this, ptr);
    }

    Iter end()
    {
        return Iter(this, nullptr);
    }

    void clear()
    {
        destory();
    }

private:
    IndexNode* next_index(IndexNode* ptr)
    {
        ptr = BsTree<size_t>::next(ptr);

        while(ptr != nullptr)
        {
            assert(array_[ptr->data_] != nullptr); // 没有删除，一定是有效的
            if (array_[ptr->data_]->status_ != KindOfEntry::Deleted)
                break;

            ptr = BsTree<size_t>::next(ptr);
        }
        return ptr;
    }

    // 入参为hash的返回值, 返回数据实际的序号, 或者应该插入的序号
    // 探测的方式为 f = +- i^2
    Pair<size_t, Entry*> find(const T& key, size_t hash_val)
    {
        if (array_ == nullptr)
            return Pair<size_t, Entry*>(0, nullptr);

        size_t index = hash_val % array_len_;
        size_t index2 = index; // 反向遍历的位置记录

        bool odd = false;      // 用于+- 探测的转换
        size_t k = 0;          // 平方探测的次数
        Entry* entry_ptr = array_[index];
        if (entry_ptr == nullptr)
            return Pair<size_t, Entry*>(index, nullptr);

        // 这里的循环能够跳出源于已经证明的结论
        while((entry_ptr->status_ == KindOfEntry::Used && alg::neq(entry_ptr->data_.first, key))
             || entry_ptr->status_ == KindOfEntry::Deleted)
        {
            if (!odd) // 正向
            {
                k++;
                index = (index + (k << 2) -1) % array_len_; // 根据递推公式得到的探测距离， 1,4,9,16...
                entry_ptr = array_[index];
            }
            else // 反向
            {
                size_t tag_val = k << 2 - 1;
                while (index2 < tag_val)           // 需要while循环, 避免+=后index2还是过小
                    index2 += array_len_;
                index2 = index2 - tag_val;         // 根据递推公式得到的探测距离， -1,-4,-9,-16...
                entry_ptr = array_[index2];
            }

            if (entry_ptr == nullptr) break;
            odd = !odd;
        }
        return Pair<size_t, Entry*>(odd ? index2 : index, entry_ptr);
    }

    bool insert(Entry* entry, size_t pos)
    {
        if (ele_size_ == 0)
        {
            size_t new_len = expand_array_len();
            array_ = reinterpret_cast<Entry**>(buy_array(new_len));
            array_len_ = new_len;
            pos = entry->hash_val_ % array_len_;
        }
        else
        {
            assert(array_[pos] == nullptr);
        }
        auto ins_pair = index_tree_.c_insert(pos);
        assert(ins_pair.first == true);          // 由平方探测结果可知，一定可以插入成功
        entry->iter_ptr_ = ins_pair.second;      // 保存插入索引节点的指针
        array_[pos] = entry;
        ele_size_ ++;

        if (ele_size_ + delete_ele_size_ >= (array_len_ * 50 / 100))
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
        return true;
    }

    // 如果内部重新哈希，则返回新数组的数据首位置索引指针，否则返回nullptr
    IndexNode* remove(Entry* entry)
    {
        assert(entry->status_ == KindOfEntry::Used);
        entry->status_ = KindOfEntry::Deleted;
        delete_ele_size_ ++;
        ele_size_ --;
        IndexNode* first_index_node = nullptr;
        if (k_num_ > 5 && ele_size_ << 3 < array_len_) // 小于表的1/8, 缩表一半
        {
            size_t new_len = retrench_array_len();
            first_index_node = handle_rehash(new_len);
        }

        if (ele_size_ == 0)
            index_tree_.clear();

        return first_index_node;
    }

    // 返回新序列中的最小索引节点指针
    IndexNode* handle_rehash(size_t new_len)
    {
        assert(new_len > ele_size_);
        Entry** new_array = reinterpret_cast<Entry**>(buy_array(new_len));
        Entry* ptr = nullptr;

        std::swap(new_array, array_);
        std::swap(new_len, array_len_);
        index_tree_.clear();

        IndexNode* first_index_ptr = nullptr;
        size_t min_index = new_len;
        for (size_t i = 0; i < new_len; i++)
        {
            ptr = new_array[i];
            if (ptr != nullptr)
            {
                if (ptr->status_ == KindOfEntry::Used)
                {
                    auto index_pair = find(ptr->data_.first, ptr->hash_val_);
                    assert(index_pair.second == nullptr);
                    auto ins_pair = index_tree_.c_insert(index_pair.first); // 重新插入序号
                    assert(ins_pair.first == true);
                    if (index_pair.first < min_index)
                    {
                        first_index_ptr = ins_pair.second;
                        min_index = index_pair.first;
                    }
                    ptr->iter_ptr_ = ins_pair.second;      // 更新索引节点的指针
                    array_[index_pair.first] = ptr;
                }
                else // Deleted
                {
                    assert(ptr->status_ == KindOfEntry::Deleted);
                    free_entry(ptr);
                }
            }
        }
        delete_ele_size_ = 0;
        if (new_array != nullptr)
            free_array(new_array);

        return first_index_ptr;
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
            // size_t next_min_num = array_len_ * 45 * 4 / 100;    // 用于测试
            // assert(ele_size_ > (array_len_ * 45 / 100));        // 扩展条件
            size_t next_min_num = ele_size_ * 4;
            do
            {
                new_array_len = 4 * ++k_num_ + 3;
            }while(new_array_len < next_min_num || !is_prime_num(new_array_len));
        }
        // stream << "k: " << k_num_ << ", new_array_len: " << new_array_len << std::endl;
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
            index_tree_.clear();
        }
    }

private:
    std::size_t  ele_size_{0};           // 有效元素的数量
    std::size_t  delete_ele_size_{0};    // 已经被删除的元素数量
    Entry**      array_{nullptr};        // 使用指针数组，扩展散列表时更加高效

    std::size_t  k_num_{5};              // 表大小计算所使用的倍数k, 初始化为5
    std::size_t  array_len_{0};          // 数组长度 满足 4k + 3 的素数

    /*
        使用一棵排序二叉树来存储所有的index序号。仅插入，不删除, 在rehash时重置. 使用排序树是为了
        在遍历时做到按照数组的序号递增地遍历，局部性友好
    */
    BsTree<size_t>   index_tree_;
};


}
#endif