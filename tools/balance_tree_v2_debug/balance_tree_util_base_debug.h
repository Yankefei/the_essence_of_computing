#ifndef _TOOLS_BALANCE_TREE_UTIL_BASE_V2_DEBUG_H_
#define _TOOLS_BALANCE_TREE_UTIL_BASE_V2_DEBUG_H_

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

    enum class Dir
    {
        Unknown,
        Left,
        Right
    };

public:
    BalanceTree_Util(int m) : m_(m), m_half_(m % 2 == 0 ? m/2 : m/2 + 1)  // m_half 取上确界
    {
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

public:
    int         m_;          // B树的阶
    int         m_half_;     // 阶的一半，计算大量使用到
};

}

#endif