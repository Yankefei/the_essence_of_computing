#include <iostream>

#include "stream.h"
#include <cassert>

#include <algorithm>
#include "algorithm.hpp"
#include "rand.h"
#include "vector.hpp"
#include "timer_guard.h"

using namespace tools;

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
        
        assert(alg::binary_find(array3, 20, 17) == -1);
        assert(alg::binary_find(array3, 20, 1000) == -1);
        assert(alg::binary_find(array3, 20, 15) == -1);
        for (int i = 0; i < 20; i ++)
        {
            assert(alg::binary_find(array3, 20, array3[i]) >= 0);
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
