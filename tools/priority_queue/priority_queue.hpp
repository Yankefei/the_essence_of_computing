#ifndef _TOOLS_PRIORITY_QUEUE_H_
#define _TOOLS_PRIORITY_QUEUE_H_

#include <memory>
#include <cassert>
#include <cmath>
#include <cstring>

#include "algorithm.hpp"
#include "../vector.hpp"

namespace tools
{

/*
    优先级队列, 默认模板参数为最大堆, 需要最小堆的功能需要修改模板参数。是以Vector容器作为底层存储的
    一个容器适配器。模板参数内，将Vector作为一个普通类型，而非像map.hpp中将Tree作为的一个模板类型。
*/
template<typename T,
         typename Compare = alg::less<T>,// less 默认建立最大堆， greater 建立最小堆
         typename Container = Vector<T>>
class PQueue
{
    typedef std::size_t       size_type;
public:
    PQueue() : array_{T()} /* 插入0号位置一个空值 */ {}

    ~PQueue() {}

    PQueue(const PQueue& rhs) : array_(rhs.array_) {}

    PQueue& operator=(const PQueue& rhs)
    {
        if (this != &rhs)
        {
            array_ = rhs.array_;
        }
        return *this;
    }

    PQueue(PQueue&& rhs) : array_{T()}
    {
        std::swap(std::move(array_), std::forward<Container>(rhs.array_));
    }

    PQueue& operator=(PQueue&& rhs)
    {
        if (this != &rhs)
        {
            if (!empty())
            {
                array_.clear();
                array_.push_back(T());   
            }
            std::swap(std::move(array_), std::forward<Container>(rhs.array_));
        }
        return *this;
    }

    void pop()
    {
        if (empty()) return;
 
        std::swap(array_[1], array_.back()); // 先和最后面的最小值交换
        array_.pop_back();
        fix_down(1);
    }

    void push(const T& val)
    {
        array_.push_back(val);
        fix_up(array_.size() - 1);
    }

    void emplace(T&& val)
    {
        array_.emplace_back(std::forward<T>(val));
        fix_up(array_.size() - 1);
    }
    
    const T& top()
    {
        return array_[1];
    }

    size_t size()
    {
        return array_.size() - 1;
    }

    bool empty()
    {
        return size() == 0;
    }

    void order()
    {
        for (auto it = array_.begin() + 1; it != array_.end(); it ++)
        {
            stream << *it << " ";
        }
        stream << std::endl;
    }

private:
    // 自底向上堆化
    void fix_up(size_type k)
    {
        size_type p_k = 0;
        while (k > 1)
        {
            p_k = k / 2;
            if (Compare()(array_[p_k], array_[k]))
            {
                std::swap(array_[p_k], array_[k]);
                k = p_k;
            }
            else
                break;
        }
    }

    // 自顶向下堆化
    void fix_down(size_type k)
    {
        size_type tag_child = k << 1;
        size_type ele_size = size();
        while (tag_child <= ele_size)
        {
            if (tag_child < ele_size &&
                Compare()(array_[tag_child], array_[tag_child + 1]))
                tag_child += 1;

            if (Compare()(array_[k], array_[tag_child]))
            {
                std::swap(array_[k], array_[tag_child]);
                k = tag_child;
                tag_child = k << 1;
            }
            else
                break;
        }
    }

private:
    Container   array_;  // 数组  数据从1号索引开始存储 array_[0] 留空
};

}

#endif
