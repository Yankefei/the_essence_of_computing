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

// 只要重载小于运算符, 那么其余的比较运算符都可以推导出来
template<typename T>
bool le(const T& lhs, const T& rhs)
{
    return lhs < rhs;
}

template<typename T>
bool eq(const T& lhs, const T& rhs)
{
    return !le(lhs, rhs) && !le(rhs, lhs);
}

template<typename T>
bool le_eq(const T& lhs, const T& rhs)
{
    return le(lhs, rhs) || eq(lhs, rhs);
}

template<typename T>
bool gt(const T& lhs, const T& rhs)
{
    return !le_eq(lhs, rhs);
}

template<typename T>
bool gt_eq(const T& lhs, const T& rhs)
{
    return !le(lhs, rhs);
}

template<typename T>
bool neq(const T& lhs, const T& rhs)
{
    return !eq(lhs, rhs);
}

template<typename T>
T max(const T& lhs, const T& rhs)
{
    return gt(lhs, rhs) ? lhs : rhs;
}

template<typename T>
T min(const T& lhs, const T& rhs)
{
    return le(lhs, rhs) ? lhs : rhs;
}

// 可调用对象版
template<typename T>
struct less
{
    bool operator()(const T& t1, const T& t2) const
    {
        return le(t1, t2);
    }
};

template<typename T>
struct greater
{
    bool operator()(const T& t1, const T& t2) const
    {
        return gt(t1, t2);
    }
};


/////////////////排序算法部分///////////////////

template<typename T>
void check_sort(T* const array, size_t src_size)
{
    size_t test_i = 0;
    size_t old_test = test_i;
    test_i ++;
    for (; test_i < src_size; test_i ++)
    {
        if (alg::gt(array[old_test], array[test_i]))
        {
            assert(false);
        }
        old_test ++;
    }
}

/**
 * 各种排序算法的脉络大值划分：(此划分脉络参考严蔚敏的划分标准，但不严谨，因为各种排序有相互借鉴)
 * 1. 基于插入的排序：   简单插入排序，希尔排序
 * 2. 基于交换的排序：   冒泡排序，快速排序
 * 3. 基于选择的排序：   简单选择排序，堆排序
 * 4. 基于分治的排序：   归并排序
 * 5. 其他的排序方案：   基数排序
*/
#define LeftChild(i) ( 2 * (i) + 1)
template<typename T>
void fix_down(T* const array, size_t begin_index, size_t end_index)
{
    size_t tag_chile = LeftChild(begin_index);
    while (tag_chile <= end_index)
    {
        if (tag_chile < end_index &&
            alg::le(array[tag_chile], array[tag_chile + 1]))
            tag_chile += 1;
        
        if (alg::le(array[begin_index], array[tag_chile]))
        {
            std::swap(array[begin_index], array[tag_chile]);
            begin_index = tag_chile;
            tag_chile = LeftChild(begin_index);
        }
        else
            break;
    }
}

/*堆排序 非稳定性排序*/
template<typename T>
void heap_sort(T* const src, size_t src_size)
{
    // 建立最大堆, 循环一半即可, 从最后一个父节点开始
    for (auto i = src_size / 2; i > 0; i--) // size_t 不能判断为 >=0, 因为恒等
    {
        fix_down(src, i - 1, src_size - 1);
    }

    // 转移然后调整堆
    for (size_t index = src_size - 1; index > 0; index --)// size_t 不能判断为 >=0, 因为恒等
    {
        std::swap(src[0], src[index]);
        fix_down(src, 0, index - 1);
    }
}

/*选择排序 稳定性排序*/
// 每次选出右边的最小值和最左边的元素交换
template<typename T>
void selection(T* const array, size_t src_size)
{
    int min_index = 0;
    for (int i = 0; i < src_size; i++)
    {
        min_index = i;
        for (int j = i; j < src_size; j++)
        {
            if (alg::le(array[j], array[min_index]))
                min_index = j;
        }
        std::swap(array[i], array[min_index]);
    }
}

/*插入排序 稳定排序*/
// 每次将待排序的左边元素插入到已经排好序的部分
template<typename T>
void insertion(T* const array, size_t src_size)
{
    int insert_index = 0;
    // i表示插入的位置
    for (int i = 0; i < src_size; i++)
    {
        // 从右向左插入
        insert_index = i;
        for (int j = i - 1; j >= 0; j--)
        {
            if (alg::le(array[insert_index], array[j]))
            {
                std::swap(array[insert_index], array[j]);
                insert_index = j;
            }
            else
                break;
        }
    }
}

/*优化后的插入排序 稳定排序*/
// 使用一个额外的存储单位保存待插入的值，减少交换的次数
template<typename T>
void insertion2(T* const array, size_t src_size)
{
    // i表示插入的位置
    for (int i = 0; i < src_size; i++)
    {
        // 从右向左插入
        T insert_val = array[i];
        int j = i - 1;
        for (; j >= 0; j--)
        {
            if (alg::le(insert_val, array[j]))
            {
                array[j + 1] = array[j];
            }
            else
            {
                array[j + 1] = insert_val;
                break;
            }
        }
        if (j < 0)
            array[0] = insert_val;
    }
}

/*优化后的插入排序 稳定排序*/
// 使用一个额外的存储单位保存待插入的值，减少交换的次数
// 提前在开头放置一个最小元素，可以减少后面循环体，让运行更快
template<typename T>
void insertion3(T* const array, size_t src_size)
{
    int min_index = src_size - 1;
    for (int i = min_index - 1; i >= 0; i--)
    {
        if (alg::le(array[i], array[min_index]))
            min_index = i;
    }
    std::swap(array[min_index], array[0]);

    // i表示插入的位置
    for (int i = 2; i < src_size; i++)
    {
        // 从右向左插入
        T insert_val = array[i];
        int j = i - 1;
        while(alg::le(insert_val, array[j]))
        {
            array[j + 1] = array[j];
            j--;
        }
        array[j + 1] = insert_val;
    }
}

/*冒泡排序 稳定排序*/
template<typename T>
void bubble(T* const array, size_t src_size)
{
    for (int i = 0; i < src_size; i++)
    {
        for (int j = src_size - 1; j > i; j--)
        {
            if (alg::le(array[j], array[j -1]))
                std::swap(array[j], array[j - 1]);
        }
    }
}

/*冒泡排序2 增加了有序即退出的机制，稳定排序*/
template<typename T>
void bubble2(T* const array, size_t src_size)
{
    bool is_sort = false;
    for (int i = 0; i < src_size && !is_sort; i++)
    {
        is_sort = true;
        for (int j = src_size - 1; j > i; j--)
        {
            if (alg::le(array[j], array[j -1]))
            {
                std::swap(array[j], array[j - 1]);
                is_sort = false;
            }
        }
    }
}

/*抖动排序 双向冒泡 稳定*/
template<typename T>
void bubble3(T* const array, size_t src_size)
{
    int i = 0;
    int j = src_size - 1;
    bool is_sort = false;
    for (; i <= j;)
    {
        for (int t_j = j; t_j > i; t_j --)
        {
            if (alg::le(array[t_j], array[t_j - 1]))
            {
                std::swap(array[t_j], array[t_j - 1]);
            }
        }
        ++i;
        for (int t_i = i; t_i < j; t_i ++)
        {
            if (alg::gt(array[t_i], array[t_i + 1]))
            {
                std::swap(array[t_i], array[t_i + 1]);
            }
        }
        --j;
    }
}

/*希尔排序 不稳定*/
/*步长公式 k = 3k + 1  1,4,13,40,121...*/
template<typename T>
void shellsort(T* const array, size_t src_size)
{
    int k = 1;
    for (; k < src_size; k = 3*k + 1) {}

    int insert_index = 0;
    for (; k >= 1; k = (k-1)/3)
    {
        for (int i = k - 1; i < src_size; i = i + k)
        {
            // 从右向左插入
            T insert_val = array[i];
            int j = i - k;
            for ( ; j >= 0; j = j -k)
            {
                if (alg::le(insert_val, array[j]))
                {
                    array[j + k] = array[j];
                }
                else
                {
                    array[j + k] = insert_val;
                    break;
                }
            }

            if (j < 0)
                array[j + k] = insert_val;
        }
    }
}


// 划分的值为右边第一位
template<typename T>
int partition(T* const array,  int left, int right)
{
    T temp_val = array[right];
    int i = left;
    int j = right;
    while(i < j)
    {
        while(i < j && alg::le_eq(array[i], temp_val))
            i++;
        while(i < j && alg::gt_eq(array[j], temp_val))
            j--;

        if (i >= j) break;

        std::swap(array[i], array[j]); 
    }
    std::swap(array[j], array[right]);
    return i;
}

/*初级快速排序 非稳定性排序*/
// 递归
template<typename T>
void _fast_sort(T* const array, int left, int right)
{
    if (left >= right) return;
    int middle = partition(array, left, right);

    _fast_sort(array, left, middle - 1);
    _fast_sort(array, middle + 1, right);
}


template<typename T>
void _fast_sort(T* const array, size_t src_size)
{
    _fast_sort(array, 0, src_size - 1);
}

#define CutOff 10

/*优化的快速排序 非稳定性排序*/
/**
 *  优化方向有两部分:
 *  1. 划分元素选取小数组中的开头，结尾，中间三者的中间值
 *  2. 小数组长度小于某个阈值(CutOff)，则跳过，结尾采用插入排序统一处理
*/
template<typename T>
void fast_sort(T* const array, int left, int right)
{
    if (left + CutOff > right) return;
    
    int center = (left + right) / 2;
    if (alg::le(array[center], array[left]))
        std::swap(array[center], array[left]);

    if (alg::le(array[right], array[left]))
        std::swap(array[right], array[left]);

    if (alg::le(array[right], array[center]))
        std::swap(array[right], array[center]);
    
    // 交换后：array[left] <= array[center] <= array[right]
    
    // 让center的值放在最右边，配合partition使用
    std::swap(array[center], array[right]);

    int middle = partition(array, left, right);

    fast_sort(array, left, middle - 1);
    fast_sort(array, middle + 1, right);
}

/*优化后的快速排序 非稳定性排序*/
template<typename T>
void fast_sort(T* const array, size_t src_size)
{
    fast_sort(array, 0, src_size - 1);
    insertion3(array, src_size);
}

/*寻找出中位序号为k的元素，如果查找中位数 k = (left+right)/2, 使用快速排序中划分数据的思想*/
/*  联想出的相关问题：
    1. 寻找数据前10%的最大值，则仅仅将k设置到[left, right]区间的90%的值即可。

    2. 寻找出middle_size个中位数, 只需要找出这部分数据的左右区间端点值，然后先用左区间端点值
    划分一次，排除掉一部分数据，然后再用右区间的值划分剩下的一部分数据，就得到了目标区间的所有值。
    需要划分两次即可。

    3. 如果难度升级, 数据太大，一台服务器无法保存，存储在100个服务器上的100个数组中，如何寻找中
    值？（前10%和middle_size个中位数同理）。
        可以这样做：
        a. 任选一个值作为枢纽值, 下发给100个服务器中进行划分。每个服务器得到大于该值的数量m1, m2
           m3, m4...和小于该值的数量n1, n2, n3, n4...。
        b. 把这些信息收集回来累加m(x)和n(x). 如果两边的数量恰好一致，则直接返回。否则进入下一步
        c. 在数量大的一边再选一个值作为枢纽值，去划分对应的那边的数据，然后再把新的m1, m2..和新
           的n1, n2, n3...进行收集和累加。然后比较两边的值，不断重复上面的过程，直到m(x)和n(x)
           的值一致为止。
        （注意：很关键的一点是，上面的过程中，每一次待划分的数组对象是在不断缩小的。但是每一次进行
        收集和累加的m(x), n(x), 都是针对整个数组而言的，m(x) + n(x)的值等于全部数组的和，而并非
        m(x)，n(x)也随着划分而不断缩小）
*/
template<typename T>
void select(T* const array, int left, int right, int k)
{
    if (left >= right) return;
    
    int middle = partition(array, left, right);
    if (middle > k)
        return select(array, left, middle - 1, k);

    if (middle < k)
        return select(array, middle + 1, right, k);
}

// from src_one and src_two to dst, no copy
template<typename T>
void merge(T* dst, T* src_one, T* src_one_end,
                         T* src_two, T* src_two_end)
{
    while(src_one != src_one_end && src_two != src_two_end)
    {
        if (alg::le(*src_two, *src_one))
        {
            *dst = *src_two++;
        }
        else
        {
            *dst = *src_one++;
        }
        dst ++;
    }
    while(src_one != src_one_end)
    {
        *dst ++ = *src_one ++;
    }
    while(src_two != src_two_end)
    {
        *dst ++ = *src_two ++;
    }
}

// 内存的使用，最终结果需要汇集到array中，暂存于temp_array中, to_array表示从temp_array
// 合并到array，还是从array合并到temp_array. 这样最终仅重复使用O(n)的空间，无拷贝。
template<typename T>
void merge_sort(T* array, int left, int right, T* temp_array, bool to_array)
{
    if (left < right)
    {
        int middle = (left + right) / 2;
        merge_sort(array, left, middle, temp_array, !to_array);
        merge_sort(array, middle + 1, right, temp_array, !to_array);
        if (to_array)
        {
            // 从array和temp_array合并到array中去
            merge(array + left, temp_array + left, temp_array + middle + 1,
                                temp_array + middle + 1, temp_array + right + 1);
        }
        else
        {
            // 从array和temp_array合并到temp_array中去
            merge(temp_array + left, array + left, array + middle + 1,
                                     array + middle + 1, array + right + 1);
        }
    }
    else if (left == right)
    {
        // 如果到了最后一层，那么无需从temp_array转移到array中，只需将array转移到temp_array中
        if (!to_array)
        {
            *(temp_array + left) = *(array + left);
        }
    }
}

/*归并排序 稳定性排序*/
template<typename T>
void merge_sort(T* const array, size_t src_size)
{
    // 申请额外的存储空间
    T* temp_array = new T[src_size];
    merge_sort(array, 0, src_size - 1, temp_array, true);
    delete[] temp_array;
}

/*stl 标准排序的一部分说明*/
/**
 * stl使用的是一种结合了插入排序，堆排序，快速排序的算法衍生物，有如下特点
 * 1. 大数据块，使用快排，如果发现数据区间小于_S_threshold（16）时，退出。
 * 2. 如果快排堆栈过深，超出阈值__depth_limit，使用同样时间复杂度的堆排序。这里的堆栈深度
 *    __depth_limit表示待排序的区间长度值的二进制最高位比特的序号，如1024是11，16是5。应
 *    该对等于：区间在随机分布下能被快速排序算法划分的次数，说明堆排序只是对付一些极端场景。
 * 3. 最后使用插入排序将大部分已经有序，只有小部分区间需要调整顺序的内容处理。
 * 
 * 其中针对快速排序的优化，本文代码针对小区间的有涉及到，另外插入排序的优化本文insertion3
 * 函数也有部分涉及到。
 * 
 * 参考: gcc源码 && https://zhuanlan.zhihu.com/p/364361964
*/

/*蒂姆排序 Java Android操作系统的排序算法的一部分说明*/
/**
 * 蒂姆排序是结合了归并排序以及插入排序，让最坏时间复杂度达到O(NlogN)， 同时保持了排序的稳定性。
 * 其中，算法核心是归并排序，在处理小区间时（小于某一个阈值）使用到了插入排序。主要利用了随机序列
 * 中，通常存在很多局部递增，递减序列的特点，实际相邻两个数总是一大一小出现的可能性很低。如果能够
 * 将这些小序列依次使用归并排序合并，将能够大大加快排序效率，算法主要设计思想如下：
 * 
 * 1. 设置一个阈值，如果小区间的长度太短，则通过带有二分查找的插入排序现将其合并为一个可以合并的
 *    最小单位。(为什么使用二分的插入排序？以我理解，找到位置后还是需要挪动数据才能完成插入，可能
 *    这样做可以减少数据比较的次数吧)
 * 
 * 2. 合并以及整理数据块的同步进行，先合并两个最短的，再逐步和其他数据块合并。这个过程使用到了栈，
 *    在入栈的过程中，始终要保证从栈底到栈顶的数据块长度是递减的，并且从栈顶开始s[0]+ s[1] < s[2]
 *    && s[0] < [1].不满足就需要调整和合并。随着整理数据块的推进，栈底序列的长度逐步增长，当整理
 *    数据完备后，会对栈中的剩余序列做最后合并，该合并方式是对归并排序的一个优化。
 *     
 * 3. 归并过程中，空间利用率除栈外，理想情况为N/2, 因为其仅开辟待合并序列最小长度的临时空间，并将
 *    数据拷贝后，合并到另一个序列中。（虽然节约空间，但该合并过程存在数据的拷贝）
 * 
 * 4. 具体合并两个序列过程中，使用一种跳跃式(galloping)的方式预测下一个合并点。具体来说，当A序列
 *    中连续若干个(设定阈值)元素都比B序列首元素小，则进入该模式。在该模式中，会通过2^k(k=1,2,3..)
 *    这样指数的方式扩大搜索范围，然后再内部使用二分查找到A中首个不小于B序列首元素的值。直接进行大块
 *    区间转移地合并，如果后面发现待转移地区间长度小于阈值，则退出该模式。这样合并方式同样是对归并
 *    排序的一个优化，不再是传统的顺序扫描方式。
 * 
 * 参考：
 *   1. 《计算之魂》1.4节
 *   2. https://github.com/python/cpython/blob/main/Objects/listsort.txt
 *   3. https://www.cnblogs.com/sunshuyi/p/12680918.html
 *   4. https://www.sakuratears.top/blog/排序算法(6)-TimSort.html (里面有详细地合并序列过程以及动画演示)
*/

/*二分查找, 返回寻找到数据的位置下标*/
template<typename T>
int binary_find(T* const array, size_t src_size, const T& val)
{
    int left = 0;
    int right = src_size - 1;
    int middle;
    while (left <= right)
    {
        middle = (left + right) / 2;
        if (alg::le(array[middle], val))
            left = middle + 1;
        else if (alg::le(val, array[middle]))
            right = middle - 1;
        else
            return middle;
    }
    return -1;
}

}
}

#endif