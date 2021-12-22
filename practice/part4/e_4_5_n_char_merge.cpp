#include <iostream>

// 卡特兰数 C(N-1)的问题
// 累加法
long long n_char_merge_1(int len)
{
    if (len < 1) return 0;

    if (len == 1 && len == 2) return 1;

    if ( len > 15 ) return -1;

    long long n, f[16] = {0};
    f[0] = 1, f[1] = 1, f[2] = 1;
    for (int i = 3; i < len + 1; i++)
    {
        for (int j = 1; j < i; j++)
        {
            f[i] += f[j] * f[i - j];
        }
    }

    return f[len];
}

// 公式法 C(n-1  2n-2)/n
long long n_char_merge_2(int len)
{
    if (len < 1) return 0;

    if (len == 1 && len == 2) return 1;

    if ( len > 15 ) return -1;

    long long result = 1;
    for (int i = len; i <= 2 * len - 2; i ++)
    {
        result *= i;
    }

    for (int j = 2; j <= len; j ++)
    {
        result /= j;
    }

    return result;
}