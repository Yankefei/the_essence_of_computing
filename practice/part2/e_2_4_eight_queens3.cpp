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


// only_check_down 只检测下半部分是否正常
bool is_valid(char chess[][8], int line, int column, bool only_check_down = false)
{
    for (int i = 0; i < 8; i++)
    {
        if (i == column) continue;
        if (chess[line][i] == 1)
        {
            std::cout << "line is not pass" << std::endl;
            return false;
        }
    }

    for (int j = 0; j < 8; j ++)
    {
        if (j == line) continue;
        if (chess[j][column] == 1)
        {
            return false;
        }
    }

    {
        if (!only_check_down)
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
        if (!only_check_down)
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

bool is_valid(char chess[][8])
{
    for (int i = 0; i < 8; i ++)
    {
        for (int j = 0; j < 8; j ++)
        {
            if (chess[i][j] == 1)
            {
                if (!is_valid(chess, i, j)) return false;
            }    
        }
    }

    return true;
}

/*
    每列全部摆放完成，再检测
*/
void eight_queens_1(char chess[][8], int line)
{
    if (line < 0)
    {
        if (is_valid(chess))
            g_find_num ++;
        return;
    }

    for (int i = 0; i < 8; i++)
    {
        memset(chess + line, 0, 8);
        chess[line][i] = 1;
        eight_queens_1(chess, line - 1);
    }
}

// 只检测line行及之下的行
bool is_valid2(char chess[][8], int line)
{
    for (int i = line; i < 8; i ++)
    {
        for (int j = 0; j < 8; j ++)
        {
            if (chess[i][j] == 1)
            {
                if (!is_valid(chess, i, j, false)) return false;
            }    
        }
    }

    return true;
}

/*
    每行边摆放边检测，已实现
    这就是回溯法？！
    完美的递归方式，只有检测成功了，才往下走，如果下面的检测失败，那么就将修改的就地还原
    使用is_valid来判断是否开始进入循环，后面好好思考下这个方法
*/
void eight_queens_2(char chess[][8], int line)
{
    if (line < 0)
    {
        g_find_num ++;
        return;
    }

    for (int i = 0; i < 8; i++)
    {
        if (is_valid(chess, line, i, true))
        {
            chess[line][i] = 1;
            eight_queens_2(chess, line - 1);
            chess[line][i] = 0;
        }
    }

    // 仅仅清理最后赋值的，还是不够, why?  面对一个本身是不好的算法，没有必要深究到底是哪里有问题
    // 过程本身太复杂，问题的难度会陡升
    // if (!last_pass) memset(chess + line, 0, 8);

    // 原来是方法用错了, 使用回溯法, 很容易
}

}

void eight_queues3()
{
    char cheer[8][8] = {0};

    //eight_test_3::eight_queens_1(cheer, 7);
    eight_test_3::eight_queens_2(cheer, 7);

    std::cout << "find: "<< eight_test_3::g_find_num << std::endl;
}