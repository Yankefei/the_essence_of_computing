#include <iostream>

#include "stream.h"


// 杨氏表格，内部可以有0， 表示这一位数字为空
int young_array[5][5] =
{
    {3, 6, 8, 10, 13},
    {4, 9, 20, 21, 30},
    {13, 14, 100, 0, 150},
    {15, 16, 100, 130, 160},
    {20, 21, 101, 130, 161}
};

extern void sort_young_table(int array[][5], int m);

int main()
{
    sort_young_table(young_array, 5);
    return 0;
}