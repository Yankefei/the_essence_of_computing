#include "stream.h"
#include <string.h>

using namespace tools;

int array[12] = {5, 2, 7, 0, 0, 8, 7, 1, 1, 2, 5, 6};

void get_sub_array(int* array, int len, int k)
{
    // 总量数组
    int* total_array = new int[len];
    memset(total_array, 0, len * sizeof(int));

    // 减去k的状态数组
    int* total_sub_array = new int[len];
    memset(total_sub_array, 0, len * sizeof(int));

    int total = 0;
    for (int i = 0; i < len ; i++)
    {
        total += array[i];
        total_array[i] = total;
        total_sub_array[i] = total - k;
    }

    int* total_ptr = total_array;
    for (int i = 0; i < len; i++)
    {
        while (total_sub_array[i] > *total_ptr)
        {
            total_ptr ++;
        }

        if (total_sub_array[i] == *total_ptr)
        {
            // 打印实际的数组起始位置
            stream << "begin: " << array[total_ptr - total_array + 1] << std::endl;
        }
    }

    delete[] total_sub_array;
    delete[] total_array;
}

void array_test()
{
    char array[12] = {5, 2, 7, 0, 0, 8, 7, 1, 1, 2, 5, 6};
    char array2[] = {5, 2, 7, 0, 0, 8, 7, 1, 1, 2, 5, 6};
    const char* array3 = "527008711256";   // 不加const 会有警告

    stream << "sizeof array " << sizeof(array) << std::endl; // 12
    stream << "sizeof array2 " << sizeof(array2) << std::endl;  // 12
    stream << "sizeof array3 " << sizeof(array3) << std::endl;  // 8

    stream << "sizeof *array " << sizeof(*array) << std::endl;  // 1
    stream << "sizeof *array2 " << sizeof(*array2) << std::endl;  // 1
    stream << "sizeof *array3 " << sizeof(*array3) << std::endl; // 1

    stream << "num: " << sizeof(array)/sizeof(char) << std::endl;  // 12
    stream << "num: " << sizeof(array2)/sizeof(char) << std::endl;  // 12
    stream << "num: " << sizeof(array3)/sizeof(char) << std::endl; // 8

    stream << "strlen: " << strlen(array) << std::endl;    // error（3）   停止于第一个'\0'
    stream << "strlen: " << strlen(array2) << std::endl;   // error（3）   停止于第一个'\0'
    stream << "strlen: " << strlen(array3) << std::endl;   // 12
}

void array_test2()
{
    char array[13] = "527008711256";   // array[12]会编译报错
    char array2[] =  "527008711256";
    char array22[] = {"527008711256"};
    const char* array3 = "527008711256";   // 不加const 会有警告

    stream << "sizeof array " << sizeof(array) << std::endl; // 13
    stream << "sizeof array2 " << sizeof(array2) << std::endl;  // 13
    stream << "sizeof array22 " << sizeof(array2) << std::endl;  // 13
    stream << "sizeof array3 " << sizeof(array3) << std::endl;  // 8

    stream << "sizeof *array " << sizeof(*array) << std::endl;  // 1
    stream << "sizeof *array2 " << sizeof(*array2) << std::endl;  // 1
    stream << "sizeof *array22 " << sizeof(*array2) << std::endl;  // 1
    stream << "sizeof *array3 " << sizeof(*array3) << std::endl; // 1

    stream << "num: " << sizeof(array)/sizeof(char) << std::endl;  // 13
    stream << "num: " << sizeof(array2)/sizeof(char) << std::endl;  // 13
    stream << "num: " << sizeof(array22)/sizeof(char) << std::endl;  // 13
    stream << "num: " << sizeof(array3)/sizeof(char) << std::endl; // 8

    stream << "strlen: " << strlen(array) << std::endl;    // 12
    stream << "strlen: " << strlen(array2) << std::endl;   // 12
    stream << "strlen: " << strlen(array22) << std::endl;   // 12
    stream << "strlen: " << strlen(array3) << std::endl;   // 12
}

void get_sub_array()
{
    // array_test();

    array_test2();

    get_sub_array(array, sizeof(array)/sizeof(int), 9);
}