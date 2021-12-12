#include <iostream>
#include <cfloat>
#include <cassert>

// 选出反方向上的累加和极致的索引号
void r_get_max_element(float array[], int array_size, int& r_index)
{
    float max = 0;
    float sum = array[array_size - 1];
    if (sum > max)
    {
        max = sum;
        r_index = array_size - 1;
    }

    for (int i = array_size - 2; i >= 0; i--)
    {
        sum += array[i];
        if (sum > max)
        {
            max = sum;
            r_index = i;
        }
    }
}

float get_sub_subinterval(float array[], int array_size)
{
    float sum = 0;
    for (int i = 0; i < array_size; i ++)
    {
        sum += array[i];
    }
    return sum;
}

// 正反两边扫描法
void find_max_subinterval_4(float array[], int array_size)
{
    int l_index = 0, r_index = 0;
    float max_sub_val = -FLT_MAX;

    float max_ele_sum = 0;
    int ele_index = 0;

    float temp_sum = 0;
    int begin_index = 0;  // 可计算的初始区间
    int sub_r_index = 0;  // 反向便利的极致索引
    bool in_subinterval = false;
    for (int i = 0; i < array_size; i++)
    {
        if (array[i] < 0 && !in_subinterval)
        {
            // 所有值均为负时的处理方案
            if (array[i] > max_sub_val)
            {
                l_index = i;
                r_index = i;
                max_sub_val = array[i];
            }
            begin_index = i + 1;
            continue;
        }

        // 获取正向上累加和的极致索引号
        temp_sum += array[i];
        if (temp_sum > max_ele_sum)
        {
            max_ele_sum = temp_sum;
            ele_index = i;
        }
        in_subinterval = true;
        if (temp_sum < 0 || array_size - 1 == i)
        {
            sub_r_index = 0;
            r_get_max_element(array + begin_index, i - begin_index, sub_r_index);
            sub_r_index += begin_index;
            assert(ele_index >= sub_r_index);
            float sub_max = get_sub_subinterval(array + sub_r_index, ele_index - sub_r_index + 1);
            if (sub_max > max_sub_val)
            {
                max_sub_val = sub_max;
                l_index = sub_r_index;
                r_index = ele_index;
            }
            in_subinterval = false;
            temp_sum = 0;
            max_ele_sum = 0;
        }
    }

    std::cout << "l_index: "<< l_index + 1 << ", r_rindex: " << r_index + 1 << ", value: "<< max_sub_val << std::endl;
}
