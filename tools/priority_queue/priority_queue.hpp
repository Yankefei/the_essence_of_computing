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
         typename Compare = alg::less<T>,
         typename Container = Vector<T>>
class PQueue
{

public:
    PQueue() = default;
    ~PQueue() {}

    void pop()
    {

    }

    void push()
    {

    }

    void emplace()
    {

    }
    
    T& top()
    {

    }

    size_t size()
    {

    }

    bool empty()
    {

    }

private:
    

private:
    Container   array_;  // 数组
};

}

#endif
