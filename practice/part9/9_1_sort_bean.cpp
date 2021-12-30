#include "stream.h"

#include "queue.h"

using namespace tools;

struct Robort
{
    bool get_bean(int* ptr)
    {
        if (*ptr == 0) return false;

        beans.push_back(*ptr);
        *ptr = 0;
        return true;
    }

    bool put_bean(int* ptr)
    {
        if (*ptr == 1) return false;

        if (beans.empty()) return false;

        beans.pop();
        *ptr = 1;
        return true;
    }

    size_t get_bean_size()
    {
        return beans.size();
    }

    void set_right_line_num()
    {
        right_line_num = beans.size();
    }

    int get_right_line_num()
    {
        return right_line_num;
    }

private:
    int right_line_num = 0;
    tools::Queue<int> beans;
};

#define LINE  6
#define ROW   9

void sort_bean(int array[LINE][ROW])
{
    Robort a;
    int* start = &array[LINE - 1][0];
    int* ptr = nullptr;
    // part1 推豆子
    for (int i = LINE - 1; i > 0; i--)
    {
        ptr = start;
        while(a.get_bean(ptr))
        {
            ptr ++;
        }

        if (a.get_right_line_num() == 0)
            a.set_right_line_num();
        
        ptr -= (ROW + 1);

        // 遇到了左边界
        while(ptr >= &array[i - 1][0])
        {
            a.get_bean(ptr);
            ptr --;
        }

        ptr ++;
        size_t bean_num = a.get_bean_size();
        while(a.get_bean_size() > bean_num - a.get_right_line_num())
        {
            a.put_bean(ptr ++);
        }

        ptr += (ROW - 1);
        while(a.put_bean(ptr))
        {
            ptr --;
        }

        assert(a.get_bean_size() == 0);

        ptr = start; // 回到起点
        start = &array[i - 1][0];
    }

    ptr = start;
    // part2 分配最有一行的豆子
    // 先拾起来
    while(a.get_bean(ptr))
    {
        ptr ++;
    }

    ptr--;
    // 然后给每一列的最上面补一颗
    for(int j = a.get_right_line_num() - 1; j >= 0; j --)
    {
        while(!a.get_bean(ptr) && ptr <= &array[LINE - 1][j])
        {
            ptr += ROW;
        }

        // 恢复原始状态
        if (ptr <= &array[LINE - 1][j] && ptr > &array[0][j])
        {
            a.put_bean(ptr); ptr -= ROW;
        }

        // 处理出界的情况
        if (ptr > &array[LINE - 1][j])
            ptr -= ROW;

        a.put_bean(ptr);

        // 还原指针
        if (j >= 1)
            ptr = &array[0][j - 1];
    }

    // for (int i = 0; i < 6; i ++)
    // {
    //     for (int j = 0; j < 9; j ++)
    //     {
    //         stream << array[i][j] << " ";
    //     }
    //     stream << std::endl;
    // }

    assert(a.get_bean_size() == 0);
}
