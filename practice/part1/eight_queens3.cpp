#include <iostream>
#include <cstring>
#include <cassert>

namespace eight_test_3
{
int g_find_num = 0;

void print(char chess[][8])
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            std::cout << (int)chess[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

bool is_valid(char chess[][8], int line, int column)
{
    // 每列只有一个
    for (int j = 0; j < 8; j ++)
    {
        if (chess[j][column] == 1)
        {
            return false;
        }
    }

    {
        int b_line = line;
        int b_column = column;
        int b_end = b_line < b_column ? b_line : b_column;
        for (int r = 1; r <= b_end; r ++)
        {
            if (chess[line - r][column - r] == 1)
            {
                return false;
            }
        }

        int f_line = 7 - line;
        int f_column = 7 - column;
        int f_end = f_line < f_column ? f_line : f_column;
        for (int r = 1; r <= f_end; r ++)
        {
            if (chess[line + r][column + r] == 1)
            {
                return false;
            }
        }
    }

    {
        int b_line = line;
        int b_column = 7 - column;
        int b_end = b_line < b_column ? b_line : b_column;
        for (int r = 1; r <= b_end; r ++)
        {
            if (chess[line - r][column + r] == 1)
            {
                return false;
            }
        }

        int f_line = 7 - line;
        int f_column = column;
        int f_end = f_line < f_column ? f_line : f_column;
        for (int r = 1; r <= f_end; r ++)
        {
            if (chess[line + r][column - r] == 1)
            {
                return false;
            }
        }
    }

    return true;
}

// show all~
bool eight_queens(char chess[][8], int line)
{
    // todo
    return true;
}

}

void eight_queues3()
{
    char cheer[8][8] = {0};
    eight_test_3::eight_queens(cheer, 7);
    std::cout << "find: "<< eight_test_3::g_find_num << std::endl;
}