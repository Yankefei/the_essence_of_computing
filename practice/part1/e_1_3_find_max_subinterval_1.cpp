#include <iostream>
#include <cfloat>

void find_max_subinterval_1(float array[], int array_size)
{
    int l_index = 0, r_index = 0;
    float max_sub_val = -FLT_MAX;

    for (int i = 0; i < array_size; i++)
    {
        for (int j = i; j < array_size; j++)
        {
            float temp_val = array[i];
            for (int n = i + 1; n <= j; n++)
            {
                temp_val += array[n];
            }

            if (temp_val > max_sub_val)
            {
                l_index = i;
                r_index = j;
                max_sub_val = temp_val;
            }
        }
    }

    std::cout << "l_index: "<< l_index + 1 << ", r_rindex: " << r_index + 1 << ", value: "<< max_sub_val << std::endl;
}