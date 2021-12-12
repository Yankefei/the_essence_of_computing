#include <iostream>
#include <cfloat>

void find_max_subinterval_2(float array[], int array_size)
{
    int l_index = 0, r_index = 0;
    float max_sub_val = -FLT_MAX;

    for (int i = 0; i < array_size; i++)
    {
        float temp_val = array[i];
        if (temp_val > max_sub_val)
        {
            max_sub_val = temp_val;
            r_index = i;
            l_index = i;
        }

        for (int j = i + 1; j < array_size; j++)
        {
            temp_val += array[j];

            if (temp_val > max_sub_val)
            {
                max_sub_val = temp_val;
                r_index = j;
                l_index = i;
            }
        }
    }

    std::cout << "l_index: "<< l_index + 1 << ", r_rindex: " << r_index + 1 << ", value: "<< max_sub_val << std::endl;
}