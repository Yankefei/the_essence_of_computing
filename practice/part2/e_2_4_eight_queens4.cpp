#include <iostream>
#include "stack.h"

namespace eight_test_4
{
int g_find_num = 0;

void print(char chess[8])
{
    for (int i = 0; i < 8; i++)
    {
        std::cout << (int)chess[i] << " ";
    }
    std::cout << std::endl;
}

// 检测方法，如果两个斜边有元素，那么通过找规律，发现这两个值的差的绝对值和列差一样
bool check_chess(char chess[8], int line, int column)
{
    for (int i = 0; i < line; i ++)
    {
        if (chess[i] == -1) continue;
        if (chess[i] == column) return false;

        char diff = chess[i] - column;
        if (diff == line - i || diff == i - line)
            return false;
    }

    for (int i = line + 1; i < 8; i++)
    {
        if (chess[i] == -1) continue;
        if (chess[i] == column) return false;

        char diff = chess[i] - column;
        if (diff == line - i || diff == i - line)
            return false;
    }

    return true;
}


bool find_eight_queens(char chess[8], int line)
{
    if (line < 0)
    {
        eight_test_4::print(chess);
        g_find_num ++;
        return true;
    }

    for (int i = 0; i < 8; i++)
    {
        if (check_chess(chess, line, i))
        {
            chess[line] = i;

            if (find_eight_queens(chess, line - 1))
            {
                return true;
            }
            else
            {
                chess[line] = -1;
            }
        }
    }

    return false;
}

// 很经典的算法
void get_eight_queens(char chess[8], int line)
{
    if (line < 0)
    {
        eight_test_4::print(chess);
        g_find_num ++;
        return;
    }

    for (int i = 0; i < 8; i ++)
    {
        if (check_chess(chess, line, i))
        {
            chess[line] = i;
            get_eight_queens(chess, line - 1);
            chess[line] = -1;
        }
    }
}

// 本算法大部分自己实现了，少部分参考了这个博客
// https://blog.csdn.net/weixin_44938368/article/details/109277280
void get_eight_queens_with_no_recursive(char chess[8], int line)
{
    int i = line;
    int j = 0;

    // 如何退出循环？
    // 怎样才算循环了一遍?
    tools::Stack<int> h_j; // 向下游循环时，记录当时的j值
    while(true)
    {
        for (; j < 8; j ++)
        {
            if (check_chess(chess, i, j))
            {
                chess[i] = j;
                break;
            }
        }

        if (i < 0)
        {
            eight_test_4::print(chess);
            g_find_num ++;
        }

        if (j == 8)
        {
            if (i == line)
            {
                // 当j为8，i 重新为line时，说明全部循环一遍
                return;
            }
            // 说明这一层已经循环完备，i需要提升到上一级，j归零后准备重新开始
            j = 0; // 还原
            i ++;  // i 提升
            
            // 当i提升时有两件事情要做
            // 1. 从栈中还原最后一次的j位置信息
            // 2. 将之前层级的赋值清理掉
            int t_j = h_j.top();
            j = t_j;
            h_j.pop();
            chess[i] = -1;  // 赋值还原
        }
        else
        {
            // 将不同层级的j信息保存在栈中
            h_j.push(j + 1);
            // i进入到下一级循环，j需要从头开始，赋值为0
            i --;
            j = 0;
        }
    }
}

}

/*
    一个感悟：
    思考这种比较复杂的回溯和递归算法时，如果一开始思路有点乱
    那么一个比较好的思考方向是，先考虑1层，2层循环的情况，将基本的处理流程都打通后
    再考虑递归到N层的情况。
    基本上只要把简单的前几层考虑清楚，后面的递归算法也就实现完备了。
    因为递归算法思考的时候有一个前提，那就是当前层的逻辑只能放在当前层中处理
    如果将其他层的逻辑混进来考虑，那肯定是思路混乱的，也必然是想不出问题
*/
void eight_queens4()
{
    char chess[8] = {0};  // 一维数组完全够
    for (int i = 0 ; i < 8; i ++)
    {
        chess[i] = -1;
    }

    //eight_test_4::find_eight_queens(chess, 7);
    //eight_test_4::get_eight_queens(chess, 7);
    eight_test_4::get_eight_queens_with_no_recursive(chess, 7);

    std::cout << "find: "<< eight_test_4::g_find_num << std::endl;
}
