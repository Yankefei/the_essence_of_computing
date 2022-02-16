#include <iostream>

#include "stream.h"
#include <cassert>

#include "algorithm.hpp"
#include "rand.h"
#include "vector.hpp"

using namespace tools;

namespace tools
{
namespace alg
{
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

/*堆排序*/
template<typename T>
void heap_sort(T* const src, size_t src_size)
{
    // 建立最大堆, 循环一半即可, 从最后一个父节点开始
    for (auto i = src_size / 2; i > 0; i--)
    {
        fix_down(src, i - 1, src_size - 1);
    }

    // for (int i = 0; i < src_size; i ++)
    // {
    //     stream << src[i] << " ";
    // }
    // stream << std::endl;

    // 转移然后调整堆
    for (size_t index = src_size - 1; index > 0; index --)
    {
        std::swap(src[0], src[index]);
        fix_down(src, 0, index - 1);
    }

    size_t test_i = 0;
    size_t old_test = test_i;
    test_i ++;
    for (; test_i < src_size; test_i ++)
    {
        if (alg::gt(src[old_test], src[test_i]))
        {
            assert(false);
        }
        old_test ++;
    }
}

}
}

int main()
{
    {
        int array[10] = {9, 3, 5, 7, 2, 0, 8, 1, 6, 4};

        alg::heap_sort(array, 10);
        for (int i = 0; i < 10; i ++)
        {
            stream << array[i] << " ";
        }
        stream << std::endl;

        int array2[11] = {9, 3, 5, 7, 2, 0, 8, 10, 1, 6, 4};
        alg::heap_sort(array2, 11);
        for (int i = 0; i < 11; i ++)
        {
            stream << array2[i] << " ";
        }
        stream << std::endl;


        int num = 100;
        while(num -- > 0)
        {
            Vector<int> array;
            Rand<int> rand(1, 100000);
            for(int i = 0; i < 10000; i++)
            {
                array.push_back(rand());
            }

            alg::heap_sort(array.data(), array.size());
        }
    }

    return 0;
}
