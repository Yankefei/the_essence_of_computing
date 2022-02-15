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
            class Compare = LessTypeCompare,/*默认为less*/
         typename Container = Vector<SizeType>>
class IndexPQueue
{
    typedef   SizeType     size_type;

public:
    IndexPQueue(TypeLink link) : compare_(link) {}
    ~IndexPQueue() {}

    IndexPQueue(const IndexPQueue&) = delete;
    IndexPQueue& operator=(const IndexPQueue&) = delete;

    IndexPQueue(IndexPQueue&& rhs) : compare_(rhs.compare_)
    {
        std::swap(ele_size_, rhs.ele_size_);
        pq_array_ = std::forward<Container>(rhs.pq_array_);
        qp_array_ = std::forward<Container>(rhs.qp_array_);
    }

    IndexPQueue& operator=(IndexPQueue&& rhs)
    {
        if (this != &rhs)
        {
            ele_size_ = 0;
            pq_array_.clear();
            qp_array_.clear();

            std::swap(ele_size_, rhs.ele_size_);
            pq_array_ = std::forward<Container>(rhs.pq_array_);
            qp_array_ = std::forward<Container>(rhs.qp_array_);
            compare_ = rhs.compare_;
        }
        return *this;
    }

    void push(size_type index)
    {

    }

    size_type erase_top()
    {

    }

    void change(size_type index)
    {

    }

    void erase(size_type index)
    {

    }

    size_type size()
    {
        return ele_size_;
    }

    bool empty()
    {
        return size() == 0;
    }

private:

private:
    Compare<TypeCallHandle, TypeLink, SizeType>   compare_;     // 比较可调用对象

    size_type    ele_size_{0};   // 被索引的元素数目

    Container    pq_array_;   // 索引数组
    Container    qp_array_;   // 索引数组的索引
};

}

#endif
