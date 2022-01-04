#ifndef _TOOLS_ALGORITHM_HPP_
#define _TOOLS_ALGORITHM_HPP_

#include "stream.h"

namespace tools
{
namespace alg
{

namespace detail{
/*
kmp算法： 核心在于不回溯原始字符串的指针，而仅仅移动模式字符串

匹配过程中，主串中第i个字符与模式中第j个字符比较不等时，仅需将模式向右滑动至第k个字符
和主串的第i个字符对齐。也就是，匹配仅需从模式的第k个字符与主串中第i个字符继续进行比较
若令 next[j] = k, 则有：
next[j]表明当模式中第j个字符与主串中相应字符"失配"时，在模式中需要重新和主串中
该字符进行比较的字符的位置。由此可引出模式串的next函数的定义:

next[j] = 
1. 0, 当j = 1时
2. MAX{k| 1 < k < j 且 ‘P1...Pk-1’ = ‘Pj-k+1...Pj-1’}  此时集合不空时
3. 其他情况

next 的获取本质上是一个模式匹配问题，后面的模式匹配依赖前面的next值

A B C A B E   如 E 匹配失败，则子串挪到3的位置，也就是C处， 如果C处还是失败，则继续挪到1处，也就是A
0 1 1 1 2 3

a a a a b    如b 匹配失败，按照上面的描述，应该先挪到最后一个a， 然后是倒数第二个a, 直到第一个a.
0 1 2 3 4 

优化后为：
0 0 0 0 4
*/
template<typename T>
void get_next(const T* dst_str, size_t dst_len, T next[])
{
    size_t i = 1;
    next[0] = 0;
    size_t j = 0;

    while(i < dst_len)
    {
        if (j == 0 || dst_str[i] == dst_str[j])
        {
            ++i;         // i是不会回溯的
            ++j;
            if (dst_str[i - 1] != dst_str[j - 1])  // 进行优化处理
                next[i - 1] = j;
            else
                next[i - 1] = next[j - 1];
        }
        else
            j = next[j] - 1; // 数据结构 C语言版中有详细地证明， 且 next 的序号从0开始
    }
}


// 二维数组内部保存的状态机的状态，表示如果遇到了某些T, 则对应转移到哪个状态中
template<typename T>
void get_next2(const T* dst_str, size_t dst_len, T** next_array)
{
    // 第一行的状态从0转移到1， 而且也只有遇到*dst_str, 才会从0转移到1
    // 其他字符都停留在0
    next_array[0][*dst_str] = 1;
    int X = 0;

    // 构建状态转移图, 从已经初始化的第一个状态图的1状态，走到最后一个状态，就说明匹配结束了
    for (int j = 1; j < dst_len; j ++)
    {
        for (int c = 0; c < 256; c++)
        {
            if (dst_str[j] == static_cast<T>(c))
            {
                // 状态推进
                // next_array[状态][字符] = 下个状态
                next_array[j][c] = j + 1;
            }
            else
            {
                // 状态重启
                // 委托X计算重启位置
                next_array[j][c] = next_array[X][c];
            }
        }

        // 更新影子状态, 也就是X永远落后j一个状态
        X = next_array[X][dst_str[j]];
    }
}

} // detail

template<typename T>
size_t kmp_find(const T* src_str,  size_t src_len, const T* dst_str, size_t dst_len)
{
    size_t find = static_cast<size_t>(-1); 

    T* next = new T[dst_len];
    detail::get_next(dst_str, dst_len, next);

    size_t i = 0;
    size_t j = 0;
    while(i < src_len && j < dst_len)
    {
        if (src_str[i] == dst_str[j] || j == 0)
        {
            i++; j++;
        }
        else
        {
            j = next[j] - 1;  // next 的序号从0开始
        }

        if (j == dst_len)
        {
            find = i - dst_len;
            break;
        }
    }

    delete[] next;
    return find;
}


// 动态规划的版本：动态规划算法: 是利用过去的结果解决现在的问题
// 使用动态规划的角度解释了KMP算法的精华内容, 这个视角非常独特, 值得好好学习
template<typename T>
size_t kmp_find2(const T* src_str,  size_t src_len, const T* dst_str, size_t dst_len)
{
    size_t find = static_cast<size_t>(-1); 

    T** array = new T*[dst_len];  // not new (T*)[dst_len]
    for (int i = 0; i < dst_len; i++)
    {
        array[i] = new T[256];
        memset(array[i], 0, sizeof(T)*256);
    }

    detail::get_next2(dst_str, dst_len, array);

    size_t i = 0;
    size_t j = 0;

    for (; i < src_len; i++)
    {
        j = array[j][src_str[i]];

        if (j == dst_len)
        {
            find = i - dst_len + 1;
            break;
        }
    }

    for (int i = 0; i < dst_len; i++)
    {
        delete[] array[i];
    }
    delete[] array;

    return find;
}


template<typename T>
bool eq(const T& lhs, const T& rhs)
{
    if (lhs == rhs) return true;
    else return false;
}

template<typename T>
bool gt(const T& lhs, const T& rhs)
{
    if (lhs > rhs) return true;
    else return false;
}

template<typename T>
bool gt_eq(const T& lhs, const T& rhs)
{
    return eq(lhs, rhs) || gt(lhs, rhs);
}

template<typename T>
bool le(const T& lhs, const T& rhs)
{
    return !gt_eq(lhs, rhs);
}

template<typename T>
bool le_eq(const T& lhs, const T& rhs)
{
    return !ge(lhs, rhs);
}

template<typename T>
bool neq(const T& lhs, const T& rhs)
{
    return !eq(lhs, rhs);
}

template<typename T>
void sort(const T* src, size_t src_size, T* dst)
{
}

}
}

#endif