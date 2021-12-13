#include <iostream>

extern void find_max_subinterval_1(float array[], int array_size);

extern void find_max_subinterval_2(float array[], int array_size);

extern void find_max_subinterval_3(float array[], int array_size);

extern void find_max_subinterval_4(float array[], int array_size);

float g_array[13] = {1.5, -12.3, 3.2,         -5.5, 23.2, 3.2,     -1.4, -62.2, 44.2, 5.4, -7.8, 1.1, -4.9};

float g_array2[13] = {-1.5, -12.3, -3.2, -5.5, -23.2, -3.2, -1.4, -62.2, -44.2, -5.4, -7.8, -1.1, -4.9};

float g_array3[13] = {1.5, -12.3, 3.2, -5.5, 23.2, 3.2,    -1.4, -12.2, 34.2, 5.4, -7.8, 1.1, -4.9};

float g_array4[13] = {1.5, -12.3, 3.2, 5.5, 23.2, 3.2,    1.4, -12.2, 34.2, 5.4, 7.8, 1.1, -4.9};

extern void eight_queues1();

extern void eight_queues2();

extern void eight_queues3();

int main()
{
    eight_queues1();

    eight_queues2();

    //eight_queues3();
#if 0
    find_max_subinterval_1(g_array, sizeof(g_array)/sizeof(float));

    find_max_subinterval_2(g_array, sizeof(g_array)/sizeof(float));

    find_max_subinterval_3(g_array, sizeof(g_array)/sizeof(float));
    find_max_subinterval_4(g_array, sizeof(g_array)/sizeof(float));

    std::cout << "------------" << std::endl;

    find_max_subinterval_1(g_array2, sizeof(g_array2)/sizeof(float));

    find_max_subinterval_2(g_array2, sizeof(g_array2)/sizeof(float));

    find_max_subinterval_3(g_array2, sizeof(g_array2)/sizeof(float));
    find_max_subinterval_4(g_array2, sizeof(g_array2)/sizeof(float));
    std::cout << "------------" << std::endl;

    find_max_subinterval_1(g_array3, sizeof(g_array3)/sizeof(float));

    find_max_subinterval_2(g_array3, sizeof(g_array3)/sizeof(float));

    find_max_subinterval_3(g_array3, sizeof(g_array3)/sizeof(float));
    find_max_subinterval_4(g_array3, sizeof(g_array3)/sizeof(float));
    std::cout << "------------" << std::endl;

    find_max_subinterval_1(g_array4, sizeof(g_array4)/sizeof(float));

    find_max_subinterval_2(g_array4, sizeof(g_array4)/sizeof(float));

    find_max_subinterval_3(g_array4, sizeof(g_array4)/sizeof(float));
    find_max_subinterval_4(g_array4, sizeof(g_array4)/sizeof(float));

#endif

    return 0;
}