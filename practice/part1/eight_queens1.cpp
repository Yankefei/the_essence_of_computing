#include <iostream>
#include <cstring>
#include <cassert>

namespace eight_test_1
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
    for (int j = 8 - 1; j > line ; j --)
    {
        if (chess[j][column] == 1)
        {
            return false;
        }
    }

    // for (int j = 0; j < 8; j ++)
    // {
    //     if (chess[j][column] == 1)
    //     {
    //         return false;
    //     }
    // }

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


/*
{
    bool t_pass = true;
    for (int j = 8 - 1; j >= line ; j --)
    {
        if (chess[j][i] == 1)
        {
            t_pass = false;
            break;
        }
    }
    if (!t_pass) break;
}

{
    // 二维数组的东南方向
    int right_line = 8 - line;
    int right_column = 8 - i - 1;
    int r_end = right_column < right_line ? right_column : right_line;
    bool t_pass = true;
    for (int r = 1; r <= r_end; r ++)
    {
        if (chess[line - 1 + r][i + r] == 1)
        {
            t_pass = false;
            break;
        }
    }
    if (!t_pass) break;
}

{
    // 二维数组的西南方向
    int left_line = 8 - line;
    int left_column = i;
    bool t_pass = true;
    int l_end = left_column < left_line ? left_column : left_line;
    for (int l = 1; l <= l_end; l ++)
    {
        if (chess[line - 1 + l][i - l] == 1)
        {
            t_pass = false;
            break;
        }
    }
    if (!t_pass) break;
}
*/

/*
    如果一开始想不清楚，那么这个垃圾代码的问题将会是无穷无尽的。
    代码的细节处理太重要了， 我写代码太糙了。。。不得不承认

    今天花了一整天的事件来调试这个代码
    希望今天的教训够深刻，算法的代码怎么写以后心里要有数，否则，就只能
    一直写垃圾代码写到底了，警惕！   2021-12-13
*/

/*
    将两边的代码进行对比，才发现之前的习惯有多差劲
    1. 入参尽量和数组的访问一致，这样算法内部就不需要考虑数组访问的问题
    2. 好的代码结构，可以让代码简介，美观，思路清晰，代码量少。
       最重要的就是效率高，bug少，这才是不做无用功的本质了
*/
bool eight_queens(char chess[][8], int line)
{
    if (line <= 0)
        return true;

    bool pass = false;
    for (int i = 0; i < 8; i++)
    {
        memset(chess + (line - 1), 0, 8);
        chess[line - 1][i] = 1;

        do
        {
            // 用来排错的，因为下面的这部分处理逻辑，实在是太乱了
            // 别人无法看懂，真的是这样，那就是乱，bug简直不要太多
            // 所以先用了正常的代码，先排除这部分没问题
            // 最后证明，经过多次修复，终于改好了，唉，垃圾代码，以后不能写了
            // if (!is_valid(chess, line - 1, i))
            // {
            //     break;
            // }
        {
            bool t_pass = true;
            for (int j = 8 - 1; j >= line ; j --)
            {
                if (chess[j][i] == 1)
                {
                    t_pass = false;
                    break;
                }
            }
            if (!t_pass) break;
        }

        {
            // 二维数组的东南方向
            int right_line = 8 - line;
            int right_column = 8 - i - 1;
            int r_end = right_column < right_line ? right_column : right_line;
            bool t_pass = true;
            for (int r = 1; r <= r_end; r ++)
            {
                if (chess[line - 1 + r][i + r] == 1)
                {
                    t_pass = false;
                    break;
                }
            }
            if (!t_pass) break;
        }

        {
            // 二维数组的西南方向
            int left_line = 8 - line;
            int left_column = i;
            bool t_pass = true;
            int l_end = left_column < left_line ? left_column : left_line;
            for (int l = 1; l <= l_end; l ++)
            {
                if (chess[line - 1 + l][i - l] == 1)
                {
                    t_pass = false;
                    break;
                }
            }
            if (!t_pass) break;
        }

        pass = true;

        }while(false);

        if (!pass)
        {
            continue;
        }

        if (!eight_queens(chess, line - 1))
        {
            continue;
        }


        if (line == 8)
        {
            g_find_num ++;
            print(chess);
        }

        // 是否要返回？ 要! 先选出一个值，再考虑其他的
        return true;

    }
    // 不能少
    memset(chess + (line - 1), 0, 8);

    // 如果任何一个都选不出来，那么就应该直接返回false
    return false;
}

}

void eight_queues1()
{
    char chess[8][8] = {0};
    eight_test_1::eight_queens(chess, 8);
    std::cout << "find: "<< eight_test_1::g_find_num << std::endl;
}