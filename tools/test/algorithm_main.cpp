#include <iostream>

#include "stream.h"
#include <cassert>

#include <algorithm>
#include "algorithm.hpp"
#include "rand.h"
#include "vector.hpp"
#include "timer_guard.h"

using namespace tools;

namespace tools
{
namespace alg
{

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

/*蒂姆排序 Java Android操作系统的排序算法*/
template<typename T>
void tim_sort(T* const array, size_t src_size)
{

}

/*二分查找*/
template<typename T>
bool binary_find(T* const array, size_t src_size, const T& val)
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
            return true;
    }
    return false;
}

}
}

int main()
{
    {
        int array[10] = {9, 3, 5, 7, 2, 0, 8, 1, 6, 4};

        alg::merge_sort(array, 10);
        //alg::fast_sort(array, 10);
        for (int i = 0; i < 10; i ++)
        {
            stream << array[i] << " ";
        }
        stream << std::endl;

        int array2[11] = {9, 3, 5, 7, 2, 0, 8, 10, 1, 6, 4};
        alg::merge_sort(array2, 11);
        //alg::fast_sort(array2, 11);
        for (int i = 0; i < 11; i ++)
        {
            stream << array2[i] << " ";
        }
        stream << std::endl;

        int array3[20] = {
                    222, 16, 884, 104, 740, 789, 498, 523, 525, 950,
                    582, 198, 35, 571, 940, 965, 116, 922, 864, 407
        };
        alg::merge_sort(array3, 20);
        
        assert(alg::binary_find(array3, 20, 17) == false);
        assert(alg::binary_find(array3, 20, 1000) == false);
        assert(alg::binary_find(array3, 20, 15) == false);
        for (int i = 0; i < 20; i ++)
        {
            assert(alg::binary_find(array3, 20, array3[i]) == true);
            stream << array3[i] << " ";
        }
        stream << std::endl;

        int array4[20];
        for (int i = 0; i < 20; i++)
            array4[i] = 10;
        alg::merge_sort(array4, 20);
        for (int i = 0; i < 20; i ++)
        {
            stream << array4[i] << " ";
        }
        stream << std::endl;

        int array5[20];
        for (int i = 1; i <= 20; i++)
            array5[i-1] = i;
        alg::merge_sort(array5, 20);
        for (int i = 0; i < 20; i ++)
        {
            stream << array5[i] << " ";
        }
        stream << std::endl;

        int array6[20];
        for (int i = 1; i <= 20; i++)
            array6[i-1] = 21 - i;
        alg::merge_sort(array6, 20);
        for (int i = 0; i < 20; i ++)
        {
            stream << array6[i] << " ";
        }
        stream << std::endl;
    }

#if 0
    {
        int array3[20] = {
                    222, 16, 884, 104, 740, 789, 498, 523, 525, 950,
                    582, 198, 35, 571, 940, 965, 116, 922, 864, 407
        };
        alg::select(array3, 0, 19, 9);
        stream << array3[9] << std::endl;
        for (int i = 0; i < 20; i ++)
        {
            stream << array3[i] << " ";
        }
        stream << std::endl;


        int array4[20];
        for (int i = 0; i < 20; i++)
            array4[i] = 10;
        alg::select(array4, 0, 19, 9);
        stream << array4[9] << std::endl;

        int array5[20];
        for (int i = 1; i <= 20; i++)
            array5[i-1] = i;
        alg::select(array5, 0, 19, 9);
        stream << array5[9] << std::endl;

        int array6[20];
        for (int i = 1; i <= 20; i++)
            array6[i-1] = 21 - i;
        alg::select(array5, 0, 19, 9);
        stream << array5[9] << std::endl;
    }
    // {
    //     int b = 20;
    //     int a = 10;
    //     const int* p = &a;
    //     p = &b;
    //     //*p = 30;  // error 底层const修饰
    // }
    // {
    //     int b = 20;
    //     int a = 10;
    //     int * const p = &a;
    //     //p = &b;   // error   顶层const修饰
    //     *p = 30;
    // }
#endif

#if 1

    {
        // 正确性测试
        int num = 200;
        while(num -- > 0)
        {
            Vector<int> array;
            Rand<int> rand(1, 10000);
            for(int i = 0; i < 1000; i++)
            {
                array.push_back(rand());
            }
            Vector<int> t_array = array;
            std::sort(t_array.data(), t_array.data() + t_array.size());

            //alg::heap_sort(array.data(), array.size());
            //alg::selection(array.data(), array.size());
            //alg::insertion(array.data(), array.size());
            //alg::insertion2(array.data(), array.size());
            //alg::insertion3(array.data(), array.size());
            //alg::bubble(array.data(), array.size());
            //alg::bubble2(array.data(), array.size());
            //alg::bubble3(array.data(), array.size());
            //alg::shellsort(array.data(), array.size());
            //alg::_fast_sort(array.data(), array.size());
            //alg::fast_sort(array.data(), array.size());
            alg::merge_sort(array.data(), array.size());

            alg::check_sort(array.data(), array.size());
            for (int i = 0; i < t_array.size(); i ++)
            {
                assert(t_array[i] == array[i]);
            }
            stream << "num: " << num << " ";
        }
        stream << std::endl;
    }
#endif

#if 0
    {
        int num = 20;
        while(num -- > 0)
        {
            Vector<int> array;
            Rand<int> rand(1, 10000);
            for(int i = 0; i < 1000; i++)
            {
                array.push_back(rand());
            }
            // 寻找中值
            // alg::select(array.data(), 0, 1000, 500);
            // stream << "num---" << array[500] << " ";

            // 前10个数据
            // alg::select(array.data(), 0, 1000, 989);
            // for (int i = 989; i < 1000; i++)
            //     stream << array[i] << " ";
            // stream << std::endl;

            // 寻找10中间位置的值
            alg::select(array.data(), 0, 1000, 495);
            alg::select(array.data(), 495, 1000, 504);
            for (int i = 495; i < 505; i++)
                stream << array[i] << " ";
            stream << std::endl;
        }
    }
#endif


#if 0
    {
        // 性能测试
        TimerGuard timer("cost time: ");
        int num = 1000;
        while(num -- > 0)
        {
            Vector<int> array;
            Rand<int> rand(1, 50000);
            for(int i = 0; i < 5000; i++)
            {
                array.push_back(rand());
            }
            Vector<int> t_array = array;
            //alg::heap_sort(array.data(), array.size());
            //alg::selection(array.data(), array.size());
            //alg::insertion(array.data(), array.size());
            //alg::insertion2(array.data(), array.size());
            //alg::insertion3(array.data(), array.size());
            //alg::bubble(array.data(), array.size());
            //alg::bubble2(array.data(), array.size());
            //alg::bubble3(array.data(), array.size());
            //alg::shellsort(array.data(), array.size());
            //alg::_fast_sort(array.data(), array.size());
            alg::fast_sort(array.data(), array.size());
        }
        stream << std::endl;
    }
#endif

    return 0;
}
