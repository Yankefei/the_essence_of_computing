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


template<typename T,
        template <typename T1> class BNode,
        template <typename T2> class BEntry>
class BalanceTree_Util
{
    typedef BNode<T>  Node;
    typedef BEntry<T>  Entry; 
public:
    /*记录查询的结果*/
    struct Result
    {
        Result(Entry* ptr, int32_t i, bool tag): ptr_(ptr), i_(i), tag_(tag)
        {}
        Entry*    ptr_{nullptr};    // 指向找到的节点
                                    // 在叶子节点中，如果tag_为false, 则为nullptr
                                    // 在非叶子节点中，该ptr_不为空
        int32_t   i_{0};            // 在pt_节点中的关键字序号, 待插入或者访问
                                    // 若tag_为false, 则可以在i_位置进行插入
        bool      tag_{false};      // 查找的结果，true/false  
    };

    enum class Dir
    {
        Unknown,
        Left,
        Right
    };

public:
    BalanceTree_Util(int m) : m_(m), m_half_(m_ / 2)
    {
    }

    // 先向右兄弟节点借, 再向左兄弟节点借
    // using LendRes = Pair<bool/*lend result*/, int32_t/*first_ele_change_index, 需要更新ptr中元素的位置*/>;
    bool lend_ele(Node* ptr, int32_t debtor_index/*发起借贷的位置*/)
    {
        bool res = false;
        if (ptr == nullptr) return res;

        assert(ptr->size_ >= debtor_index + 1);
        Dir shift_dir = Dir::Unknown;
        Node* left_ptr = nullptr;
        Node* right_ptr = nullptr;
        // int32_t first_ele_change_index = 0;
        do
        {
            if (debtor_index == 0)  // 左端点
            {
                if (/*ptr->size_ == 1 ||*/ ptr->array_[debtor_index + 1].next_->size_ == m_half_)
                {
                    break;
                }
                shift_dir = Dir::Left;
                left_ptr = ptr->array_[debtor_index].next_;
                right_ptr = ptr->array_[debtor_index + 1].next_;
                // first_ele_change_index = debtor_index + 1;
            }
            else if (debtor_index == ptr->size_ - 1)  // 右端点
            {
                if (ptr->array_[debtor_index - 1].next_->size_ == m_half_)
                {
                    break;
                }
                shift_dir = Dir::Right;
                left_ptr = ptr->array_[debtor_index - 1].next_;
                right_ptr = ptr->array_[debtor_index].next_;
                // first_ele_change_index = debtor_index;
            }
            else
            {
                if (ptr->array_[debtor_index + 1].next_->size_ == m_half_)
                {
                    if (ptr->array_[debtor_index - 1].next_->size_ == m_half_)
                    {
                        break;   
                    }
                    else
                    {
                        shift_dir = Dir::Right;
                        left_ptr = ptr->array_[debtor_index -1].next_;
                        right_ptr = ptr->array_[debtor_index].next_;
                        // first_ele_change_index = debtor_index;
                    }
                }
                else
                {
                    shift_dir = Dir::Left;
                    left_ptr = ptr->array_[debtor_index].next_;
                    right_ptr = ptr->array_[debtor_index + 1].next_;
                    // first_ele_change_index = debtor_index + 1;
                }
            }
            shift_element(left_ptr, right_ptr, shift_dir);
            // 更新非叶子节点的数值, first_ele_change_index的值可以用方向来区分
            if (shift_dir == Dir::Left)
            {
                ptr->array_[debtor_index + 1].data_ = right_ptr->array_[0].data_;
            }
            else
            {
                ptr->array_[debtor_index].data_ = right_ptr->array_[0].data_;
            }
            res = true;
        }while(false);

        return res;
    }

    // shift_dir 表示转移节点的方向
    void shift_element(Node* left_ptr, Node* right_ptr, Dir shift_dir)
    {
        if (shift_dir == Dir::Left)
        {
            assert(left_ptr->size_ < m_half_ && right_ptr->size_ > m_half_);
            left_ptr->array_[left_ptr->size_++] = right_ptr->array_[0];
            for (int i = 1; i < right_ptr->size_; i++)
            {
                right_ptr->array_[i - 1] = right_ptr->array_[i];
            }
            right_ptr->size_ --;
        }
        else
        {
            assert(left_ptr->size_ > m_half_ && right_ptr->size_ < m_half_);
            for (int i = right_ptr->size_; i > 0; i--)
            {
                right_ptr->array_[i] = right_ptr->array_[i - 1];
            }
            right_ptr->size_ ++;
            right_ptr->array_[0] = left_ptr->array_[left_ptr->size_ - 1];
            left_ptr->array_[left_ptr->size_ - 1].next_ = nullptr;
            left_ptr->size_ --;
        }
    }

    // 将src_ptr的所有节点转移到 dst_ptr中, src在右，dst在左, lost_ele_dir 表示两者哪一个是删除元素的节点
    void shift_node(Node* dst_ptr, Node* src_ptr, Dir lost_ele_dir)
    {
        if (lost_ele_dir == Dir::Left)
            assert(dst_ptr->size_ < m_half_ && src_ptr->size_ == m_half_);
        else
            assert(dst_ptr->size_ == m_half_ && src_ptr->size_ < m_half_);

        int i = 0;
        for (; i < src_ptr->size_; i++)
        {
            dst_ptr->array_[dst_ptr->size_++] = src_ptr->array_[i];
        }
    }

    // 删除完毕，进行重排
    void remove_ele(Node* ptr, int32_t index)
    {
        assert(ptr->size_ >= index + 1);
        assert(ptr->array_[index].next_ == nullptr);  // 必须将待删除的节点信息全部维护完才能删除
        int i = index + 1;
        for (; i < ptr->size_; i++)
        {
            ptr->array_[i-1] = ptr->array_[i];
        }
        
        if (i != index + 1)
            ptr->array_[i - 1].next_ = nullptr;
        else
            ptr->array_[index].next_ = nullptr;
        ptr->size_ --;
    }

    Result find(Node* ptr, const T& val, int hight)
    {
        while(hight > 0)
        {
            Result res = find_next(ptr, val);
            hight --;
            ptr = res.ptr_->next_;
        }
        return find_val(ptr, val);
    }

    // 返回的Result中, 可能存在位置 i_ >= size_, 表示未来的插入位置可能不需要位置挪动, 或者在新的节点中
    Result find_val(Node* ptr, const T& val)   // pass
    {
        Result res{nullptr, 0, false};
        if (ptr == nullptr || ptr->size_ == 0) return res;
        // 二分查找大于等于该值的最小区间, 用于寻找将val插入的位置
        int begin_index = 0;
        int end_index = ptr->size_ - 1;
        if (alg::le(val, ptr->array_[begin_index].data_))
        {
            return res;
        }
        // if (alg::le(ptr->array_[end_index].data_, val))
        // {
        //     res.i_ = end_index + 1;
        //     return res;
        // }

        int index = 0;
        while(begin_index <= end_index)
        {
            index = (begin_index + end_index) / 2;
            if (alg::eq(val, ptr->array_[index].data_))
            {
                res.ptr_ = &ptr->array_[index];
                res.i_ = index;
                res.tag_ = true;
                return res;
            }
            else
            {
                if (end_index - begin_index <= 1)
                {
                    break; // 进入了末端，逐个比较
                }
                if (alg::le(val, ptr->array_[index].data_))
                    end_index = index - 1;
                else
                    begin_index = index + 1;
            }
        }

        // 到最后该选择哪个位置进行替换，还需要再判断
        int i = begin_index;
        for (; i <= end_index; i++)
        {
            if (alg::eq(val, ptr->array_[i].data_))
            {
                res.i_ = i;
                res.ptr_ = &ptr->array_[i];
                res.tag_ = true;
                return res;
            }

            if (alg::le(val, ptr->array_[i].data_))
            {
                // 返回第一个大于val的位置
                res.i_ = i;
                res.ptr_ = &ptr->array_[i];
                return res;
            }
        }
        res.i_ = i;
        res.ptr_ = &ptr->array_[i];
        return res;
    }

    // 返回的Result中, i_的位置不会超过size_的位置
    Result find_next(Node* ptr, const T& val) // pass
    {
        // 二分查找小于等于该值的最大区间，用于从区间的下层搜索val
        Result res = {nullptr, 0, false};
        int begin_index = 0;
        int end_index = ptr->size_ -1;

        assert(ptr->size_ != 0);

        // 提前处理在begin边界外的情况
        if (alg::le(val, ptr->array_[begin_index].data_))
        {
            res.ptr_ = &ptr->array_[begin_index];
            return res;
        }
        // if (alg::gt(val, ptr->array_[end_index].data_))
        // {
        //     res.ptr_ = &ptr->array_[end_index];
        //     res.i_ = end_index;
        //     return res;
        // }

        // 只要落在这个区间内，必然会有对应的指针返回
        int index = 0;
        while(begin_index <= end_index)
        {
            index = (begin_index + end_index) / 2;
            if (alg::eq(val, ptr->array_[index].data_))
            {
                res.ptr_ = &ptr->array_[index];
                res.i_ = index;
                res.tag_ = true;
                return res;
            }
            else
            {
                if (end_index - begin_index <= 1)
                {
                    break; // 进入了末端，逐个比较
                }
                if (alg::le(val, ptr->array_[index].data_))
                    end_index = index - 1;
                else
                    begin_index = index + 1;
            }
        }

        int i = end_index;
        for (; i >= begin_index; i--)
        {
            if (alg::eq(val, ptr->array_[i].data_))
            {
                res.ptr_ = &ptr->array_[i];
                res.i_ = i;
                res.tag_ = true;
                return res;
            }

            if (alg::le(ptr->array_[i].data_, val))
            {
                // 返回第一个小于val的位置
                res.i_ = i;
                res.ptr_ = &ptr->array_[res.i_];
                return res;
            }
        }

        res.i_ = i;
        res.ptr_ = &ptr->array_[res.i_];
        return res;
    }

public:
    int         m_;          // B树的阶
    int         m_half_;     // 阶的一半，计算大量使用到
};

}

#endif