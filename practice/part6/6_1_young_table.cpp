#include <iostream>

#include <cstring>

#include "stream.h"

using namespace tools;

#define COLUMN 5

int min(int a, int b)
{
    return a > b ? b : a;
}

// 当前行的索引末尾
int get_index_array_end(int row_num, int n)
{
    return n * (row_num + 1);
}

void sort_table(int *array, int temp_array[], int m, int n)
{
    // 记录每一行读取到哪个位置的数组索引
    int* d_array = temp_array;
    int* index_array = new int[m];
    for(int i = 0; i < m; i ++)
    {
        index_array[i] = i * n;
    }

    // index_array 索引的查询范围
    int min_row = 0;
    int max_row = 1;

    while(true)
    {
        if (min_row == m && max_row == m)
            break;
        
        int get_index = -1;
        int get_val = INT32_MAX;
        for (int i = min_row; i < max_row; i++)
        {
            if (array[index_array[i]] == 0)
            {
                index_array[i] ++;
                continue;
            }

            auto t_get_val = min(get_val, array[index_array[i]]);
            if (get_val != t_get_val)
            {
                get_val = t_get_val;
                get_index = i;
            }
        }

        index_array[get_index] ++;
        // stream << get_val << " ";
        *temp_array = get_val;
        temp_array ++;

        if (index_array[min_row] == get_index_array_end(min_row, n))
        {
            min_row ++;
        }
        // 判断max_row是否应该递增
        if (index_array[max_row - 1] != (max_row - 1) * n  && max_row < m)
        {
            max_row ++;
        }
    }
    // stream << std::endl;

    delete[] index_array;
}

// m * n
// 变量名优先和[]符号结合
// int *p[5];   是指针数组， 有 5 个成员，每个成员都是一个指针，共有5 个指针
// int (*p)[5]； 小括号优先，所以 是 1个指针，用来指向 有5个元素的数组。
void sort_young_table(int array[][COLUMN], int m)
{
    if (m <= 0) return;

    int num = m * COLUMN;
    int* temp_array = new int[num];
    memset(temp_array, 0, num);

    // array : int (*)[5]
    sort_table( *array, temp_array, m, COLUMN);

    for(int i = 0; i < num; i++)
    {
        stream << temp_array[i] << " ";
    }

    stream << std::endl;

    delete[] temp_array;
}

// todo
bool is_young_table(int array[][COLUMN], int m)
{
    return true;
}

void insert_young_table(int array[][COLUMN], int m, int val)
{

}

void build_young_table(int array[][COLUMN], int m, int array_val[])
{
    // use insert_young_table
}