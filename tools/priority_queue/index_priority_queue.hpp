#ifndef _TOOLS_INDEX_PRIORITY_QUEUE_H_
#define _TOOLS_INDEX_PRIORITY_QUEUE_H_

#include <memory>
#include <cassert>
#include <cmath>
#include <cstring>

#include "priority_queue.hpp"

namespace tools
{

// 索引数据的less比较对象, 建立最大堆
template <typename CallHandle, typename TypeLink, typename SizeType>
struct LessTypeCompare
{
    LessTypeCompare(const TypeLink& link) : array_(link) {}

    LessTypeCompare(const LessTypeCompare& rhs) : array_(rhs.array_) {}

    bool operator()(SizeType index1, SizeType index2)
    {
        return CallHandle()(array_, index1) < CallHandle()(array_, index2);
    }

    TypeLink  array_;
};

// 索引数据的greater比较对象, 建立最小堆
template <typename CallHandle, typename TypeLink, typename SizeType>
struct GreaterTypeCompare
{
    GreaterTypeCompare(const TypeLink& link) : array_(link) {}

    GreaterTypeCompare(const GreaterTypeCompare& rhs) : array_(rhs.array_) {}

    bool operator()(SizeType index1, SizeType index2)
    {
        return CallHandle()(array_, index1) > CallHandle()(array_, index2);
    }

    TypeLink  array_;
};

/*
    提供纯粹索引功能的优先级队列, 索引堆:
    特点：
    1. 容器内部不直接引用待排序的任何外部数据项，仅仅保存以及操作这些外部数据项的索引值。
    2. 内部排序是由单独的索引结构体来承担。
    3. 实际数据的比较方法需要实现为可调用对象并由模板参数传入，参数为索引值，返回比价结果。
    4. 支持优先级队列常规的插入，以及删除顶部数据项的功能。
    3. 可以动态删除任意地数据项索引。
    4. 支持动态修改该对应索引的外部数据，修改后调用change方法即可，参数为该修改项的索引值。

    TypeCallHandle可调用函数对象的两个参数，第一个必须是TypeLink, 第二个必须是SizeType
*/
template<typename TypeCallHandle, /*可根据TypeLink和索引值访问Type数据的可调用对象*/
         typename TypeLink, /*被索引的对象类型句柄，如指针，用于初始化比较对象*/
         typename SizeType = std::size_t, /*索引类型*/
         template <typename _TypeCallHandle, typename _TypeLink, typename _SizeType>
            class Compare = LessTypeCompare,/*默认为less 建立最大堆, GreaterTypeCompare 建立最小堆*/
         typename Container = Vector<SizeType>>
class IndexPQueue
{
    typedef   SizeType     size_type;

public:
    IndexPQueue(TypeLink link) : compare_(link),
        pq_array_{0}/* 插入0号位置一个空值*/ {}

    ~IndexPQueue() {}

    // 只能唯一引用原始数据
    IndexPQueue(const IndexPQueue&) = delete;
    IndexPQueue& operator=(const IndexPQueue&) = delete;

    // 调用移动版本的拷贝构造和赋值运算符后, 原对象失效, 不能继续使用
    IndexPQueue(IndexPQueue&& rhs) : compare_(rhs.compare_),
        pq_array_{0}/* 插入0号位置一个空值*/
    {
        std::swap(ele_size_, rhs.ele_size_);
        std::swap(max_data_index_, rhs.max_data_index_);
        std::swap(std::move(pq_array_), std::forward<Container>(rhs.pq_array_));
        std::swap(std::move(qp_array_), std::forward<Container>(rhs.qp_array_));
    }

    IndexPQueue& operator=(IndexPQueue&& rhs)
    {
        if (this != &rhs)
        {
            if (!pq_array_.empty())
            {
                pq_array_.clear(); pq_array_.push_back(0);
            }
            if (!qp_array_.empty())
            {
                qp_array_.clear();
            }
            ele_size_ = 0; max_data_index_ = UINT64_MAX;

            std::swap(ele_size_, rhs.ele_size_);
            std::swap(max_data_index_, rhs.max_data_index_);
            std::swap(std::move(pq_array_), std::forward<Container>(rhs.pq_array_));
            std::swap(std::move(qp_array_), std::forward<Container>(rhs.qp_array_));
            compare_ = rhs.compare_;
        }
        return *this;
    }

    // 索引从0位开始。首次插入时, index需要按照顺序以递增的方式插入, 否则返回失败
    // 插入重复索引，则表示原有索引指向的原始数据有变动
    bool push(size_type index)
    {
        if (ele_size_ != 0 && max_data_index_ >= index)
        {
            // 出现重复索引的情况
            if (qp_array_[index] != -1)
            {
                assert(change(index) == true);
                return true;
            }
        }
        else if (max_data_index_ + 1 == index) // 可兼容处理ele_size_ == 0的场景
        {
            max_data_index_ ++;
            qp_array_.push_back(pq_array_.size());  // 新增索引数组的最新位置, 后面pq_array_会增加元素
        }
        else if (ele_size_ == 0 || max_data_index_ < index)
            return false;

        pq_array_.push_back(index);
        ele_size_ ++;
        fix_up(ele_size_);
        return true;
    }

    // 出队失败，返回-1
    size_type erase_top()
    {
        if (empty()) return -1;

        size_type re = pq_array_[1];  // 返回了实际数据的索引
        exchange(1, ele_size_);

        qp_array_[pq_array_.back()] = -1;
        pq_array_.pop_back();

        ele_size_ --;
        fix_down(1, ele_size_);

        return re;
    }

    // 外界数据修改了内容后，通知容器进行同步序列状态
    bool change(size_type index)
    {
        if (index > max_data_index_ || qp_array_[index] == -1)
            return false;

        // 先向上堆化，再向下堆化
        fix_up(qp_array_[index]);
        fix_down(qp_array_[index], ele_size_);
        return true;
    }

    bool erase(size_type index)
    {
        if (index > max_data_index_ || qp_array_[index] == -1)
            return false;

        size_t old_pq_index = qp_array_[index];
        /*
            note: 这里需要复用 change()函数中的一部分逻辑：
            删除一个节点 = 和最后一个节点交换 + 同步调整除最后一个节点外的堆状态(change)
        */

        exchange(old_pq_index, ele_size_);

        fix_up(old_pq_index);
        fix_down(old_pq_index, ele_size_ - 1);

        qp_array_[pq_array_.back()] = -1;
        pq_array_.pop_back();

        ele_size_ --;
        return true;
    }

    // 查看原始数据是否存在于队列中
    bool data_contains(size_type index)
    {
        if (index > max_data_index_) return false;
        // 因为qp_array_记录的存在，查找过程变成了常数时间
        return qp_array_[index] != -1;
    }

    size_type size()
    {
        return ele_size_;
    }

    bool empty()
    {
        return size() == 0;
    }

    void order_index_array()
    {
        stream << "pq_array : ";
        auto it = pq_array_.begin();
        stream <<"*  ";
        it ++;
        for (; it != pq_array_.end(); it ++)
        {
            stream << *it << "  ";
        }
        stream << std::endl;

        stream << "qp_array : ";
        for (auto it = qp_array_.begin(); it != qp_array_.end(); it ++)
        {
            if (*it == -1)
                stream << "-1 ";
            else
                stream << *it << "  ";
        }
        stream << std::endl << std::endl;
    }

    bool check_index_queue()
    {
        size_t child_index = 0;
        // 检查索引堆是否所有数据的顺序都正常
        for (size_t i = 1; i <= ele_size_; i++)
        {
            assert(qp_array_[pq_array_[i]] == i);
            child_index = i << 1;
            if (child_index <= ele_size_ && compare_(pq_array_[i], pq_array_[child_index]))
            {
                assert(false);
            }
            child_index = i;
        }

        for (size_t i = 0; i < qp_array_.size(); i++)
        {
            if (qp_array_[i] == -1)
                continue;
            assert(pq_array_[qp_array_[i]] == i);
        }
        return true;
    }

private:
    // 获取以index为顶的堆的最后一个节点的位置
    size_t end_index_in_heap(size_t index)
    {
        if (index > ele_size_) return UINT64_MAX;

        size_t tag_index = index;
        size_t last_tag = 0;
        while(tag_index <= ele_size_)
        {
            last_tag = tag_index;
            tag_index = (tag_index << 1) + 1;
        }

        return last_tag;
    }

    // 自底向上堆化 k表示在pq_array数组的位置
    void fix_up(size_type k)
    {
        size_type p_k = 0;
        while (k > 1)
        {
            p_k = k / 2;
            // 用真实数据的内容指导索引数组的排序方案
            if (compare_(pq_array_[p_k], pq_array_[k])) // 由array还原为实际的数据索引
            {
                exchange(p_k, k);
                k = p_k;
            }
            else
                break;
        }
    }

    // 自顶向下堆化
    void fix_down(size_type k, size_type last_index)
    {
        size_type tag_child = k << 1;
        while(tag_child <= last_index)
        {
            if (tag_child < last_index &&
                compare_(pq_array_[tag_child], pq_array_[tag_child + 1]))
                tag_child += 1;

            if (compare_(pq_array_[k], pq_array_[tag_child]))
            {
                exchange(k, tag_child);
                k = tag_child;
                tag_child = k << 1;
            }
            else
                break;
        }
    }

    // index1 和 index2 分别表示 pq_array_数组中的下标
    // 将工具函数封装起来，可以有效增加代码的逻辑性和清晰度！
    void exchange(size_type index1, size_type index2)
    {
        assert(qp_array_[pq_array_[index1]] != -1);
        assert(qp_array_[pq_array_[index2] != -1]);

        std::swap(qp_array_[pq_array_[index1]], qp_array_[pq_array_[index2]]);
        std::swap(pq_array_[index1], pq_array_[index2]);
    }

private:
    Compare<TypeCallHandle, TypeLink, SizeType>   compare_;     // 比较可调用对象

    size_type    ele_size_{0};                 // 被索引的元素数目
    size_type    max_data_index_{UINT64_MAX};  // 原数据曾经出现过的最大索引值, 初始化为极大值
                                               // +1后变为0, 这是为了支持原始数据索引可以从0开始

    Container    pq_array_;   // 索引数组，跳过0位置，从1位置开始

    // 逆索引，可以理解为原数据在索引数组中的位置信息， 用于快速判断原数据是否存在于索引数组中
    /*
        假如往堆索引中输入了11个值的索引，删除指向较小值索引, 只保留最大的5个值的索引, 最终的效果如下：
        (如果想得到11个值的前5个最大值，最有效的方案是建立最小堆，不断从顶部出队，这里只做索引演示)
        k       qp_array    pq_array     data[k]
        ----------------------------------------                      (最大堆)
        0                                   63
        1          5           3            86                            90
        2          2           2            87                          /    \
        3          1           4            90 第一名                  87     84
        4          3           9            84                       /   \
        5                      1            65                      86   86
        6                                   82
        7                                   61
        8                                   76
        9          4                        86
        10                                  71
    
        假如现在有需要交换pq_array中位置3和位置5的值, 表示1号数据和4号数据的相对大小变动，那么只需要
        将qp_array[1]和qp_array[4]同步交换即可。
    */
    Container    qp_array_; // 从0位置开始
};

}

#endif
