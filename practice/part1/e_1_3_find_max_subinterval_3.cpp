#include "iostream"
#include <cfloat>
#include <cassert>

float get_max_val(float array[], int array_size, int& l_index_val, int& r_index_val)
{
    float max_sub_val = 0;
    if (array_size == 1)
    {
        max_sub_val = array[0];
        return max_sub_val;
    }

    int half_size = array_size / 2;

    int l_sub_l_index = 0, l_sub_r_index = 0;
    float l_max_val = get_max_val(array, half_size, l_sub_l_index, l_sub_r_index);

    int r_sub_l_index = 0, r_sub_r_index = 0;
    float r_max_val = get_max_val(array + half_size, array_size - half_size, r_sub_l_index, r_sub_r_index);
    if (array_size % 2 == 0)
    {
        r_sub_l_index += (array_size - half_size);
        r_sub_r_index += (array_size - half_size);
    }
    else
    {
        r_sub_l_index += (array_size - half_size -1);
        r_sub_r_index += (array_size - half_size -1);
    }

    // 这里要分两种情况进行考虑
    // 1种是左区间的最大子区间和右区间的最大子区间相邻
    // 2种是左区间的最大子区间和右区间的最大子区间不相邻
    // 所有的情况都要在递归的逻辑中考虑进去

    // 两个区间不连续
    if (l_sub_r_index + 1 == r_sub_l_index)
    {
        if (l_max_val > 0 && r_max_val > 0)
        {
            l_index_val = l_sub_l_index;
            r_index_val = r_sub_r_index;
            max_sub_val = l_max_val + r_max_val;
        }
        else
        {
            if (l_max_val > r_max_val)
            {
                max_sub_val = l_max_val;
                l_index_val = l_sub_l_index;
                r_index_val = l_sub_r_index;
            }
            else
            {
                max_sub_val = r_max_val;
                l_index_val = r_sub_l_index;
                r_index_val = r_sub_r_index;
            }
        }
    }
    else if (l_sub_l_index + 1 > r_sub_r_index)
    {
        assert(false);
    }
    else
    {
        // 只能获取两个区间之内的数据，不能获取边界值
        float temp_val = array[l_sub_r_index + 1];
        for (int i = l_sub_r_index + 2; i < r_sub_l_index; i ++)
        {
            temp_val += array[i];
        }
        temp_val += (l_max_val + r_max_val);

        float max_temp = l_max_val > r_max_val ? l_max_val : r_max_val;
        max_temp = max_temp > temp_val ? max_temp : temp_val;

        if (max_temp == l_max_val)
        {
            l_index_val = l_sub_l_index;
            r_index_val = l_sub_r_index;
            max_sub_val = l_max_val;
        }
        else if (max_temp == temp_val)
        {
            l_index_val = l_sub_l_index;
            r_index_val = r_sub_r_index;
            max_sub_val = temp_val;
        }
        else
        {
            l_index_val = r_sub_l_index;
            r_index_val = r_sub_r_index;
            max_sub_val = r_max_val;
        }
    }

    // std::cout <<array[0] << ", array_size: " << array_size <<  ", l: "<< l_index_val << ", r: "<< r_index_val << " max: "<< max_sub_val << std::endl;

    return max_sub_val;
}

void find_max_subinterval_3(float array[], int array_size)
{
    int l_index = 0, r_index = 0;
    float max_sub_val = get_max_val(array, array_size, l_index, r_index);

    std::cout << "l_index: "<< l_index + 1 << ", r_rindex: " << r_index + 1 << ", value: "<< max_sub_val << std::endl;
}