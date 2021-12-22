#include <iostream>

// m, n 表示n*m的 节点数
uint64_t work_n_n_1(uint32_t m, uint32_t n)
{
    if(m == 1 || n == 1) return 1;
    else return work_n_n_1(m-1, n) + work_n_n_1(m, n-1);
}


// m, n 表示n*m的 边数
uint64_t work_n_n_2(uint32_t m, uint32_t n)
{
    if(m == 0 || n == 0) return 1;

    uint64_t result = 1;
    for (int i = m + 1; i <= m + n; i ++)
    {
        result *= i;
    }

    for (int j = 1; j <= n; j++)
    {
        result /= j;
    }

    return result;
}