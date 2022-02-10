#ifndef _TOOLS_BALANCE_TREE_UTIL_BASE2_H_
#define _TOOLS_BALANCE_TREE_UTIL_BASE2_H_

#include <memory>
#include <cassert>
#include <cstring> // memset

#include "algorithm.hpp"

namespace tools
{

template<typename T,
        template <typename T1> class BNode,
        template <typename T2> class BEntry>
class BalanceTree_Util2
{
    typedef BNode<T>  Node;
    typedef BEntry<T>  Entry; 
public:
    /*记录查询的结果*/
    struct Result
    {
        Result() = default;
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
    };

    enum class Dir
    {
        Unknown,
        Left,
        Right
    };

public:
    BalanceTree_Util2(int m) : m_(m), m_half_(m % 2 == 0 ? m/2 : m/2 + 1)
    {
    }

    BalanceTree_Util2(const BalanceTree_Util2& rhs)
    {
        copy(rhs);
    }

    void copy(const BalanceTree_Util2& rhs)
    {
        m_ = rhs.m_;
        m_half_ = rhs.m_half_;
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
                if (/*ptr->size_ == 1 ||*/ ptr->array_[debtor_index + 1]->next_->size_ == m_half_)
                {
                    break;
                }
                shift_dir = Dir::Left;
                left_ptr = ptr->array_[debtor_index]->next_;
                right_ptr = ptr->array_[debtor_index + 1]->next_;
                // first_ele_change_index = debtor_index + 1;
            }
            else if (debtor_index == ptr->size_ - 1)  // 右端点
            {
                if (ptr->array_[debtor_index - 1]->next_->size_ == m_half_)
                {
                    break;
                }
                shift_dir = Dir::Right;
                left_ptr = ptr->array_[debtor_index - 1]->next_;
                right_ptr = ptr->array_[debtor_index]->next_;
                // first_ele_change_index = debtor_index;
            }
            else
            {
                if (ptr->array_[debtor_index + 1]->next_->size_ == m_half_)
                {
                    if (ptr->array_[debtor_index - 1]->next_->size_ == m_half_)
                    {
                        break;   
                    }
                    else
                    {
                        shift_dir = Dir::Right;
                        left_ptr = ptr->array_[debtor_index -1]->next_;
                        right_ptr = ptr->array_[debtor_index]->next_;
                        // first_ele_change_index = debtor_index;
                    }
                }
                else
                {
                    shift_dir = Dir::Left;
                    left_ptr = ptr->array_[debtor_index]->next_;
                    right_ptr = ptr->array_[debtor_index + 1]->next_;
                    // first_ele_change_index = debtor_index + 1;
                }
            }
            shift_element(left_ptr, right_ptr, shift_dir);
            // 更新非叶子节点的数值, first_ele_change_index的值可以用方向来区分
            if (shift_dir == Dir::Left)
            {
                ptr->array_[debtor_index + 1]->data_ = right_ptr->array_[0]->data_;
            }
            else
            {
                ptr->array_[debtor_index]->data_ = right_ptr->array_[0]->data_;
            }
            res = true;
        }while(false);

        return res;
    }

    // shift_dir 表示转移节点的方向, 转移则不需要回收内存
    void shift_element(Node* left_ptr, Node* right_ptr, Dir shift_dir)
    {
        if (shift_dir == Dir::Left)
        {
            assert(left_ptr->size_ < m_half_ && right_ptr->size_ > m_half_);
            if (right_ptr->array_[0]->next_ != nullptr)
            {
                right_ptr->array_[0]->next_->parent_ = left_ptr; // 维护父指针
            }
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
            if (left_ptr->array_[left_ptr->size_ - 1]->next_ != nullptr)
            {
                left_ptr->array_[left_ptr->size_ -1]->next_->parent_ = right_ptr; // 维护父指针
            }
            right_ptr->array_[0] = left_ptr->array_[left_ptr->size_ - 1];
            left_ptr->array_[left_ptr->size_ - 1] = nullptr;
            left_ptr->size_ --;
        }
    }

    // 将src_ptr的所有节点转移到 dst_ptr中, src在右，dst在左, lost_ele_dir 表示两者哪一个是删除元素的节点
    // 转移不需要回收内存
    void shift_node(Node* dst_ptr, Node* src_ptr, Dir lost_ele_dir)
    {
        if (lost_ele_dir == Dir::Left)
            assert(dst_ptr->size_ < m_half_ && src_ptr->size_ == m_half_);
        else
            assert(dst_ptr->size_ == m_half_ && src_ptr->size_ < m_half_);

        int i = 0;
        for (; i < src_ptr->size_; i++)
        {
            if (src_ptr->array_[i]->next_ != nullptr)
            {
                src_ptr->array_[i]->next_->parent_ = dst_ptr; // 维护父指针
            }
            dst_ptr->array_[dst_ptr->size_++] = src_ptr->array_[i];
        }
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
        if (alg::le(val, ptr->array_[begin_index]->data_))
        {
            return res;
        }
        // if (alg::le(ptr->array_[end_index]->data_, val))
        // {
        //     res.i_ = end_index + 1;
        //     return res;
        // }

        int index = 0;
        while(begin_index <= end_index)
        {
            index = (begin_index + end_index) / 2;
            if (alg::eq(val, ptr->array_[index]->data_))
            {
                res.ptr_ = ptr->array_[index];
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
                if (alg::le(val, ptr->array_[index]->data_))
                    end_index = index - 1;
                else
                    begin_index = index + 1;
            }
        }

        // 到最后该选择哪个位置进行替换，还需要再判断
        int i = begin_index;
        for (; i <= end_index; i++)
        {
            if (alg::eq(val, ptr->array_[i]->data_))
            {
                res.i_ = i;
                res.ptr_ = ptr->array_[i];
                res.tag_ = true;
                return res;
            }

            if (alg::le(val, ptr->array_[i]->data_))
            {
                // 返回第一个大于val的位置
                res.i_ = i;
                res.ptr_ = ptr->array_[i];
                return res;
            }
        }
        res.i_ = i;
        res.ptr_ = ptr->array_[i];
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
        if (alg::le(val, ptr->array_[begin_index]->data_))
        {
            res.ptr_ = ptr->array_[begin_index];
            return res;
        }
        // if (alg::gt(val, ptr->array_[end_index]->data_))
        // {
        //     res.ptr_ = ptr->array_[end_index];
        //     res.i_ = end_index;
        //     return res;
        // }

        // 只要落在这个区间内，必然会有对应的指针返回
        int index = 0;
        while(begin_index <= end_index)
        {
            index = (begin_index + end_index) / 2;
            if (alg::eq(val, ptr->array_[index]->data_))
            {
                res.ptr_ = ptr->array_[index];
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
                if (alg::le(val, ptr->array_[index]->data_))
                    end_index = index - 1;
                else
                    begin_index = index + 1;
            }
        }

        int i = end_index;
        for (; i >= begin_index; i--)
        {
            if (alg::eq(val, ptr->array_[i]->data_))
            {
                res.ptr_ = ptr->array_[i];
                res.i_ = i;
                res.tag_ = true;
                return res;
            }

            if (alg::le(ptr->array_[i]->data_, val))
            {
                // 返回第一个小于val的位置
                res.i_ = i;
                res.ptr_ = ptr->array_[res.i_];
                return res;
            }
        }

        res.i_ = i;
        res.ptr_ = ptr->array_[res.i_];
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
        默认返回的IterResult中的节点的hight都为0
        默认根节点的parent_指针为nullptr, 便于判断位置信息
    */
    // hight 为当前ptr的高度
    IterResult first(Node* ptr, int32_t hight)
    {
        IterResult res;
        if (ptr == nullptr || hight < 0) return res;
        while(hight-- > 0)
        {
            ptr = ptr->array_[0]->next_;
        }
        res.node_ptr = ptr;
        res.entry_ptr = ptr->array_[0];
        res.index = 0;
        return res;
    }

    // index 表示ptr在当前Node中的位置
    // 如果到最后一个，则返回的node_ptr为nullptr
    IterResult next(const IterResult& iter)
    {
        IterResult res;
        if (iter.node_ptr->size_ > iter.index + 1)
        {
            res.node_ptr = iter.node_ptr;
            res.index = iter.index + 1;
            res.entry_ptr = res.node_ptr->array_[res.index];
            return res;
        }

        int32_t hight = 0;
        Result f_res;
        Node* ptr = iter.node_ptr;

        while(ptr->parent_ != nullptr)
        {
            f_res = find_val(ptr->parent_, ptr->array_[0]->data_); // 一定可以找到，所以使用find_val
            assert(f_res.tag_ == true);
            ptr = ptr->parent_;
            
            if (f_res.i_ + 1 < ptr->size_)
            {
                ptr = ptr->array_[f_res.i_ + 1]->next_;
                break;
            }
            else // f_res.i_ + 1 == ptr->size_
            {
                // 当前如果没有找到下一层节点，则将hight++
                hight ++;
            }
        }

        if (ptr->parent_ == nullptr)
            return res;

        return first(ptr, hight);
    }

    IterResult last(Node* ptr, int32_t hight)
    {
        IterResult res;
        if (ptr == nullptr || hight < 0) return res;
        while(hight-- > 0)
        {
            ptr = ptr->array_[ptr->size_ - 1]->next_;
        }
        res.node_ptr = ptr;
        res.entry_ptr = ptr->array_[ptr->size_ - 1];
        res.index = ptr->size_ - 1;
        return res;
    }

    IterResult prev(const IterResult& iter)
    {
        IterResult res;
        if (0 <= iter.index - 1)
        {
            res.node_ptr = iter.node_ptr;
            res.index = iter.index - 1;
            res.entry_ptr = res.node_ptr->array_[res.index];
            return res;
        }

        int32_t hight = 0;
        Result f_res;
        Node* ptr = iter.node_ptr;

        while(ptr->parent_ != nullptr)
        {
            f_res = find_val(ptr->parent_, ptr->array_[0]->data_); // 一定可以找到，所以使用find_val
            assert(f_res.tag_ == true);
            ptr = ptr->parent_;
            
            if (f_res.i_ -1 >= 0)
            {
                ptr = ptr->array_[f_res.i_ - 1]->next_;
                break;
            }
            else // f_res.i_ - 1 < 0
            {
                // 当前如果没有找到下一层节点，则将hight++
                hight ++;
            }
        }

        if (ptr->parent_ == nullptr)
            return res;

        return last(ptr, hight);
    }

    // 调试成功，感想：程序的模块化思想无敌...  - 2022.2

    // 判断是否为一棵符合规范的B树
    /*
        1. 有序
        2. 节点元素数 m/2 <= x <= m, 根除外（2 <= x <= m）
        3. 所有叶子节点等高
        4. 非叶子节点的元素均需要出现在叶子节点中
        5. m个元素的非叶子节点下游存在m个节点
        6. 非叶子节点中保存的元素值均为下层叶子节点中最小值
    */
    bool is_b_tree(Node* ptr, int hight)
    {
        if (ptr == nullptr)
        {
            assert(hight == -1);
            return true;  //空节点默认是B数
        }
        if (hight_ == 0 && ptr && ptr->size_ == 0)
            return true;   // 删除全部数据后，空节点也算是

        assert(hight >= 0);
        assert(ptr->size_ <= m_);
        assert(ptr->size_ > 0);

        bool res = false;
        if (hight_ > 0)
        {
            if (hight == hight_)
            {
                if (ptr->size_ < 2)  // 根节点的元素数
                    return res;
            }
            else
            {
                if (ptr->size_ < m_half_)
                    return res;
            }
        }

        do
        {
            int j = 0;
            int i = 0;
            for (; i < ptr->size_; i++)
            {
                assert(ptr->array_[i] != nullptr);
                if (hight > 0)
                {
                    assert(ptr->array_[i]->next_ != nullptr);
                    if (!alg::eq(ptr->array_[i]->data_, ptr->array_[i]->next_->array_[0]->data_))
                        break;
                    
                    if (i > 0)
                    {
                        int n_size = ptr->array_[j]->next_->size_;
                        // 当前的值比前一位值的下级节点的最大值还要大，保证有序
                        if (!alg::le(ptr->array_[j]->next_->array_[n_size - 1]->data_, ptr->array_[i]->data_))
                        {
                            break;
                        }
                    }
                }
                if (i > 0)
                {
                    if (!alg::gt(ptr->array_[i]->data_, ptr->array_[j]->data_))
                        break;
                }
                j = i; // 让j落后i一位

                if (!is_b_tree(ptr->array_[i]->next_, hight - 1))
                    break;
            }

            if (i < ptr->size_)
                break;
            res = true;
        }while(false);

        if (!res)
        {
            // draw_tree<Node>(ptr, hight, m_);
        }

        return res;
    }

public:
    int         m_;          // B树的阶
    int         m_half_;     // 阶的一半，计算大量使用到

    int         hight_{-1};  // 树高, 单节点的B树默认树高为0, 空树默认树高为-1
};

}

#endif