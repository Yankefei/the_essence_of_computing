#ifndef _TOOLS_BALANCE_TREE_UTIL_BASE_V2_DEBUG2_H_
#define _TOOLS_BALANCE_TREE_UTIL_BASE_V2_DEBUG2_H_

#include <memory>
#include <cassert>
#include <cstring> // memset

#include "algorithm.hpp"

namespace tools
{

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

    // 迭代器遍历的结果
    struct IterResult
    {
        IterResult() = default;

        Node*    node_ptr{nullptr};
        Entry*   entry_ptr{nullptr};
        int32_t  index{0};
        int32_t  hight{0};
    };

    enum class Dir
    {
        Unknown,
        Left,
        Right
    };

public:
    BalanceTree_Util(int m) : m_(m), m_half_(m % 2 == 0 ? m/2 : m/2 + 1)
    {
    }

    T& get_last_data_in_node(Node* ptr)
    {
        assert(ptr != nullptr && ptr->size_ > 0);
        return ptr->array_[ptr->size_ - 1].data_;
    }

    // 将next_node的最后一个元素的值赋值到ptr指针指向Node的index位置
    void no_leaf_node_add_ele(Node* ptr, int32_t index, Node*next_node)
    {
        ptr->array_[index].data_ = get_last_data_in_node(next_node);
        // 因为这个Entry很快要脱离索引范围, 所以进行清理，赋值T()主要是为了在GDB下观察
        // 内存数据直观期间，实际可以忽略该步骤，其他地方赋值T()主要也是这个作用
        get_last_data_in_node(next_node) = T();
        // 保证之前的操作彻底，不要残留数据
        assert(next_node->array_[next_node->size_].next_ == nullptr);
        next_node->size_ --;
    }


    // 不考虑尾部的指针, 因为该函数仅会处理叶子节点
    void remove_ele(Node* ptr, int32_t index)
    {
        for (int i = index; i < ptr->size_ - 1; i++)
        {
            ptr->array_[i] = ptr->array_[i + 1];
        }
        ptr->size_ --;
        ptr->array_[ptr->size_].next_ = nullptr;
        ptr->array_[ptr->size_].data_ = T();
    }

    // 考虑叶子节点和非叶子节点两种情况
    // 租借节点，如果左右兄弟节点均可，则先向右兄弟借，再向左兄弟借
    bool lend_ele(Node* pptr, int32_t debtor_index/*发起借贷的位置*/)
    {
        assert(debtor_index <= pptr->size_);
        bool res = false;
        Dir shift_dir = Dir::Unknown; // 借贷的方向
        Node* left_ptr = nullptr;
        Node* right_ptr = nullptr;

        do
        {
            if (debtor_index == 0) // 左端点
            {
                if (pptr->array_[debtor_index + 1].next_->size_ == m_half_ -1)
                {
                    break;
                }
                shift_dir = Dir::Left;
                left_ptr = pptr->array_[debtor_index].next_;
                right_ptr = pptr->array_[debtor_index + 1].next_;
            }
            else if (debtor_index == pptr->size_) // 右端点
            {
                if (pptr->array_[debtor_index - 1].next_->size_ == m_half_ -1)
                {
                    break;
                }
                shift_dir = Dir::Right;
                left_ptr = pptr->array_[debtor_index - 1].next_;
                right_ptr = pptr->array_[debtor_index].next_;
            }
            else
            {
                if (pptr->array_[debtor_index + 1].next_->size_ == m_half_ -1)
                {
                    if (pptr->array_[debtor_index - 1].next_->size_ == m_half_ -1)
                    {
                        break;
                    }
                    else
                    {
                        shift_dir = Dir::Right;
                        left_ptr = pptr->array_[debtor_index - 1].next_;
                        right_ptr = pptr->array_[debtor_index].next_;
                    }
                }
                else
                {
                    shift_dir = Dir::Left;
                    left_ptr = pptr->array_[debtor_index].next_;
                    right_ptr = pptr->array_[debtor_index + 1].next_;
                }
            }
            shift_element(pptr, debtor_index, left_ptr, right_ptr, shift_dir);

            res = true;
        }while(false);

        return res;
    }

    void shift_element(Node* pptr, int32_t index, Node* left_ptr, Node* right_ptr,
                       Dir shift_dir)
    {
        if (shift_dir == Dir::Left)
        {
            left_ptr->array_[left_ptr->size_].data_ = pptr->array_[index].data_;
            pptr->array_[index].data_ = right_ptr->array_[0].data_;
            // 需要前置++, 不能直接覆盖原有的尾部next_指针
            left_ptr->array_[++left_ptr->size_].next_ = right_ptr->array_[0].next_;
            if (left_ptr->array_[left_ptr->size_].next_ != nullptr)
            {
                left_ptr->array_[left_ptr->size_].next_->parent_ = left_ptr;
            }
            // 将right_ptr的节点挪动, 包含尾部指针
            for (int i = 0; i < right_ptr->size_; i++)
            {
                right_ptr->array_[i] = right_ptr->array_[i + 1];
            }
            // 清理残留数据
            right_ptr->array_[right_ptr->size_].next_ = nullptr;
            right_ptr->array_[--right_ptr->size_].data_ = T(); // 前置--
        }
        else
        {
            // 将right_ptr的节点挪动位置，空出0位置，包含尾部指针
            // 后置++
            for (int i = right_ptr->size_++; i >= 0; i--)
            {
                right_ptr->array_[i + 1] = right_ptr->array_[i];
            }
            right_ptr->array_[0].data_ = pptr->array_[index - 1].data_;
            right_ptr->array_[0].next_ = left_ptr->array_[left_ptr->size_].next_;
            if (right_ptr->array_[0].next_ != nullptr)
            {
                right_ptr->array_[0].next_->parent_ = right_ptr;
            }
            pptr->array_[index - 1].data_ = left_ptr->array_[left_ptr->size_ - 1].data_;
            // 清理残留数据
            left_ptr->array_[left_ptr->size_].next_ = nullptr;
            left_ptr->array_[--left_ptr->size_].data_ = T();   // 前置--
        }
    }

    // 获取非叶子节点元素的前一个叶子节点元素Entry地址
    Entry* get_last_ele_from_leaf(Node* ptr, int32_t hight)
    {
        while(hight-- > 0)
        {
            ptr = ptr->array_[ptr->size_].next_;
        }
        return &(ptr->array_[ptr->size_ -1]);
    }

    Result find(Node* ptr, const T& val, int hight)
    {
        while(hight > 0)
        {
            Result res = find_next(ptr, val);
            if (res.tag_)
                return res;
            hight --;
            ptr = res.ptr_->next_;
        }
        return find_val(ptr, val);
    }

    // 如果未找到，则返回将要插入的目标位置，原有位置的数据券后向后挪动
    Result find_val(Node* ptr, const T& val)
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

    // 如果未找到，则返回可能携带该值的next_指针的序号位置
    Result find_next(Node* ptr, const T& val)
    {
        // 二分查找小于等于该值的最大区间，用于从区间的下层搜索val
        Result res = {nullptr, 0, false};
        int begin_index = 0;
        int end_index = ptr->size_ - 1;

        assert(ptr->size_ != 0);

        // 提前处理在end边界外的情况
        if (alg::le(ptr->array_[end_index].data_, val))
        {
            res.i_ = end_index + 1;
            res.ptr_ = &ptr->array_[res.i_];
            return res;
        }

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
                // 返回第一个小于val的位置, 由于该next_指针在i+1的位置,所以需要+1
                res.i_ = i + 1;
                res.ptr_ = &ptr->array_[res.i_];
                return res;
            }
        }

        res.i_ = i + 1;  // 做同样处理
        res.ptr_ = &ptr->array_[res.i_];
        return res;
    }

    size_t in_order(Node* root, int32_t hight)
    {
        size_t e_size = 0;
        IterResult begin = first(root, hight);
        if (begin.node_ptr == nullptr) return e_size;
        Entry* f_min = begin.entry_ptr;
        for (; begin.node_ptr != nullptr; begin = next(begin))
        {
            if (alg::le(begin.entry_ptr->data_, f_min->data_))
                assert(false);
            
            f_min = begin.entry_ptr;
            // stream << begin.entry_ptr->data_ << " ";
            e_size ++;
        }
        // stream << std::endl;
        return e_size;
    }

    size_t prev_in_order(Node* root, int32_t hight)
    {
        size_t e_size = 0;
        IterResult end = last(root, hight);
        if (end.node_ptr == nullptr) return e_size;
        Entry* f_max = end.entry_ptr;
        for (; end.node_ptr != nullptr; end = prev(end))
        {
            if (alg::le(f_max->data_, end.entry_ptr->data_))
                assert(false);

            f_max = end.entry_ptr;
            // stream << end.entry_ptr->data_ << " ";
            e_size ++;
        }
        // stream << std::endl;
        return e_size;
    }

    /*
        默认根节点的parent_指针为nullptr, 便于判断位置信息
    */
    // hight 为当前ptr的高度
    IterResult first(Node* ptr, int32_t hight)
    {
        IterResult res;
        if (ptr == nullptr || hight < 0) return res;
        while(hight > 0)
        {
            ptr = ptr->array_[0].next_;
            hight --;
        }
        res.node_ptr = ptr;
        res.entry_ptr = &(ptr->array_[0]);
        res.index = 0;
        res.hight = hight;
        return res;
    }

    // index 表示ptr在当前Node中的位置
    // 如果到最后一个，则返回的node_ptr为nullptr
    IterResult next(const IterResult& iter)
    {
        IterResult res;
        Node* ptr = iter.node_ptr;

        if (ptr)
        {
            if (iter.hight == 0)
            {
                if (iter.index < ptr->size_ - 1)
                {
                    res.node_ptr = ptr;
                    res.index = iter.index + 1;
                    res.hight = iter.hight;
                    res.entry_ptr = &(ptr->array_[res.index]);
                }
                else
                {
                    // Node* old_ptr = ptr;
                    Node* pa = ptr->parent_;
                    int32_t hight = 1;
                    while(pa != nullptr && pa->array_[pa->size_].next_ == ptr)
                    {
                        ptr = pa;
                        pa = pa->parent_;
                        hight ++;
                    }

                    // 已经是最后一个，直接返回空的res
                    if (pa == nullptr) return res;
                    // 任意查找一个即可
                    Result f_res = find_next(pa, ptr->array_[0].data_);
                    assert(f_res.tag_ == false);

                    res.node_ptr = pa;
                    res.index = f_res.i_;
                    res.hight = hight;
                    res.entry_ptr = &(pa->array_[res.index]);
                }
                return res;
            }
            else
            {
                return first(ptr->array_[iter.index + 1].next_, iter.hight - 1);
            }
        }
        return res;
    }

    IterResult last(Node* ptr, int32_t hight)
    {
        IterResult res;
        if (ptr == nullptr || hight < 0) return res;
        while(hight > 0)
        {
            ptr = ptr->array_[ptr->size_].next_;
            hight--;
        }
        res.node_ptr = ptr;
        res.entry_ptr = &(ptr->array_[ptr->size_ - 1]);
        res.index = ptr->size_ - 1;
        res.hight = hight;
        return res;
    }

    IterResult prev(const IterResult& iter)
    {
        IterResult res;
        Node* ptr = iter.node_ptr;
        if (ptr)
        {
            if (iter.hight == 0)
            {
                if (iter.index > 0)
                {
                    res.node_ptr = ptr;
                    res.index = iter.index - 1;
                    res.hight = iter.hight;
                    res.entry_ptr = &(ptr->array_[res.index]);
                }
                else
                {
                    Node* pa = ptr->parent_;
                    int32_t hight = 1;
                    while(pa != nullptr && pa->array_[0].next_ == ptr)
                    {
                        ptr = pa;
                        pa = pa->parent_;
                        hight ++;
                    }

                    // 已经是第一个，直接返回空的res
                    if (pa == nullptr) return res;
                    // 任意查找一个即可
                    Result f_res = find_next(pa, ptr->array_[0].data_);
                    assert(f_res.tag_ == false);

                    res.node_ptr = pa;
                    res.index = f_res.i_ - 1;
                    res.hight = hight;
                    res.entry_ptr = &(pa->array_[res.index]);
                }
                return res;
            }
            else
            {
                return last(ptr->array_[iter.index].next_, iter.hight - 1);
            }
        }

        return res;
    }

public:
    int         m_;          // B树的阶
    int         m_half_;     // 阶的一半，计算大量使用到
};

}

#endif