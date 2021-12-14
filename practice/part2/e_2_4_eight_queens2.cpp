#include <iostream>
#include <cstring>
#include <cassert>

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
        if (j == line) continue;
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

bool is_valid(char chess[][8])
{
    for (int i = 0; i < 8; i ++)
    {
        for (int j = 0; j < 8; j ++)
        {
            if (chess[i][j] == 1)
            {
                if (!is_valid(chess, i, j))
                {
                    std::cout << i << " " << j << std::endl;
                    return false;
                }
            }
        }
    }

    return true;
}

/*
    处处体现着逆向思维
    1. 只有先确定合法，再赋值，比赋值后再判断是否合法要少做很多事情
       起码就不需要再把赋值的值修改回去，而且也专注于合法之后的逻辑。
    2. 只有成功才返回true, 其他所有情况均不考虑，直接返回false.
    
    这里的递归为什么能成功？
    **经验和教育太重要了**
    1. 逻辑清晰，不能说因为有了递归，就可以不管内部的逻辑
    2. 先把第一层的问题理清楚了之后，再扎到递归里面
       而不是先一头扎到递归里面，再考虑具体的逻辑，这样逻辑就很混乱了
       把本层未处理的问题，全部放在本层来解决完，不要让下面的层去处理
    3. 对算法的边界把握地很到位，如果一开始脑子晕，可以先一个算法一次只
       处理一个问题
       比如这里，只选出正确的一个值，至于是否完全，可以先不管。
*/
bool eight_queens(char chess[][8], int line)
{
    if (line < 0)
    {
        g_find_num ++;
        return true;
    }

    for (int i = 0; i < 8; i++)
    {
        if (is_valid(chess, line, i))
        {
            chess[line][i] = 1;

            if (eight_queens(chess, line - 1))
            {
                return true;
            }
            else
            {
                chess[line][i] = 0;
            }
        }
    }

    return false;
}


void eight_queues2()
{
    char chess[8][8] = {0};
    eight_queens(chess, 7);
    print(chess);
    // if (is_valid(chess))
    //     std::cout << "pass" << std::endl;
    std::cout << "find: "<< g_find_num << std::endl;
}