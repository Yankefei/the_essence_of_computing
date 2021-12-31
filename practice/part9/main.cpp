#include <iostream>

#include "stream.h"

using namespace tools;

int array[6][9] =
   {{0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 1, 1, 0, 0},
    {0, 1, 0, 1, 1, 1, 1, 0, 0},
    {1, 1, 0, 1, 1, 1, 1, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 0, 0}};

extern void sort_bean(int array[6][9]);

int main()
{
    for (int i = 0; i < 6; i ++)
    {
        for (int j = 0; j < 9; j ++)
        {
            stream << array[i][j] << " ";
        }
        stream << std::endl;
    }

    sort_bean(array);
    stream << "after sort" << std::endl;
    for (int i = 0; i < 6; i ++)
    {
        for (int j = 0; j < 9; j ++)
        {
            stream << array[i][j] << " ";
        }
        stream << std::endl;
    }

    return 0;
}