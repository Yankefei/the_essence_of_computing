#include "stream.h"
#include "general.h"
#include <assert.h>

using namespace tools;


int array_one[10] = {281, 320, 335, 339, 419, 603, 614, 650, 710, 720};

int array_two[12] = {334, 420, 445, 620, 632, 685, 715, 740, 779, 780, 810, 812};

struct RangInfo
{
    int* begin = nullptr;
    int index = 0;
    int num = 0;
};

// 获取begin_one 和begin_two 两个数组指针中下一个最大值，修改后，当前的指针需要++
int* get_next_ptr(int*& begin_one, int*& begin_two, int* end_one, int* end_two)
{
    int* ptr = nullptr;
    if (begin_one > end_one)
        ptr = begin_two ++;
    else if (begin_two > end_two)
        ptr = begin_one ++;
    else if (*begin_one > *begin_two)
        ptr = begin_two ++;
    else
        ptr = begin_one ++;

    return ptr;
}

// k个区间中寻找最大范围
void get_snippets_generation(int array[], size_t num, int array2[], size_t num2, int k)
{
    // 用于k区间起点的推进
    int* curr_one = array;
    int* curr_two = array2;

    // 用于k区间终点的推进
    int* begin_one = array;
    int* begin_two = array2;
    RangInfo history_r;
    if (array[0] < array2[0])
    {
        history_r.begin = array;
        history_r.index = array[0];
        history_r.num ++;
        begin_one ++;
        curr_one++;
    }
    else
    {
        history_r.begin = array2;
        history_r.index = array2[0];
        history_r.num ++;
        begin_two ++;
        curr_two ++;
    }

    RangInfo current = history_r;
    int* end_one = array + num - 1;
    int* end_two = array2 + num2 - 1;

    int * tag_index = nullptr;

    while(begin_one <= end_one || begin_two <= end_two)
    {
        tag_index = get_next_ptr(begin_one, begin_two, end_one, end_two);

        if (*tag_index - *current.begin < k)
        {
            current.num ++;
            continue;
        }

        // 置换
        if (current.num > history_r.num)
            history_r = current;

        // current.begin 和 tag_index 的递增规律完全一致，所以如果前者追赶后者，则将会一致
        while (current.begin != tag_index && *tag_index - *current.begin >= k)
        {
            current.begin = get_next_ptr(curr_one, curr_two, end_one, end_two);
            current.num --;
        }

        if (current.begin != tag_index)
            current.num ++;
        else
        {
            //assert(current.begin == tag_index);
            current.num = 1;
        }
    }

    if (current.num > history_r.num)
        history_r = current;
    
    stream << "begin: " << *history_r.begin << ", num: " << history_r.num << std::endl;
}


void get_rang_info()
{
    get_snippets_generation(array_one, 10, array_two, 12, 50);
}