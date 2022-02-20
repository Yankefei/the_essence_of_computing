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

template<typename T>
void fix_down(T* const array, size_t begin_index, size_t end_index)
{
    size_t tag_chile = begin_index << 1;
    while (tag_chile <= end_index)
    {
        if (tag_chile < end_index &&
            alg::le(array[tag_chile], array[tag_chile + 1]))
            tag_chile += 1;
        
        if (alg::le(array[begin_index], array[tag_chile]))
        {
            std::swap(array[begin_index], array[tag_chile]);
            begin_index = tag_chile;
            tag_chile = begin_index  << 1;
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
    for (auto i = src_size / 2; i > 0; i--)
    {
        fix_down(src, i - 1, src_size - 1);
    }

    // 转移然后调整堆
    for (size_t index = src_size - 1; index > 0; index --)
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

/*寻找出middle_size个中位数, 以及快速排序*/
template<typename T>
void select(T* const array, size_t src_size, size_t middle_size)
{

}

/*归并排序 稳定性排序*/
/**/


/*stl 标准排序*/

/*二分查找*/
template<typename T>
bool find(T* const array, size_t src_size, const T& val)
{

}

}
}

int main()
{
    {
        int array[10] = {9, 3, 5, 7, 2, 0, 8, 1, 6, 4};

        //alg::heap_sort(array, 10);
        alg::fast_sort(array, 10);
        for (int i = 0; i < 10; i ++)
        {
            stream << array[i] << " ";
        }
        stream << std::endl;

        int array2[11] = {9, 3, 5, 7, 2, 0, 8, 10, 1, 6, 4};
        //alg::heap_sort(array2, 11);
        alg::fast_sort(array2, 11);
        for (int i = 0; i < 11; i ++)
        {
            stream << array2[i] << " ";
        }
        stream << std::endl;

        int array3[20] = {
                    222, 16, 884, 104, 740, 789, 498, 523, 525, 950,
                    582, 198, 35, 571, 940, 965, 116, 922, 864, 407
        };
        alg::fast_sort(array3, 20);
        for (int i = 0; i < 20; i ++)
        {
            stream << array3[i] << " ";
        }
        stream << std::endl;

        int array4[20];
        for (int i = 0; i < 20; i++)
            array4[i] = 10;
        alg::fast_sort(array4, 20);
        for (int i = 0; i < 20; i ++)
        {
            stream << array4[i] << " ";
        }
        stream << std::endl;

        int array5[20];
        for (int i = 1; i <= 20; i++)
            array5[i-1] = i;
        alg::fast_sort(array5, 20);
        for (int i = 0; i < 20; i ++)
        {
            stream << array5[i] << " ";
        }
        stream << std::endl;

        int array6[20];
        for (int i = 1; i <= 20; i++)
            array6[i-1] = 21 - i;
        alg::fast_sort(array6, 20);
        for (int i = 0; i < 20; i ++)
        {
            stream << array6[i] << " ";
        }
        stream << std::endl;
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
            alg::fast_sort(array.data(), array.size());

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
