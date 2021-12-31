#include "stream.h"

#include <cstring>

using namespace tools;

int g_line_index = 1;  // 以序号为1的行开始
char array[30][5] = {0};

int g_ele_num = 0;

// 方法1
// 递归打印
void Print(const char* ptr, int len, int line_index)
{
    if (len == 0)
    {
        stream <<"print: "<< line_index<< " " << array[line_index] << std::endl;
        g_ele_num ++;
    }
    else
    {
        size_t l_len = strlen(array[line_index]);

        memcpy(array[line_index* 2], array[line_index], l_len);
        array[line_index * 2][l_len] = *ptr;
        Print(ptr + 1, len - 1, line_index * 2);

        memcpy(array[line_index* 2 + 1], array[line_index], l_len);
        Print(ptr + 1, len - 1, line_index * 2 + 1);
    }
}

char tmp[100];

// 方法2
// 递归打印, 仅使用一维数组来保存最终的值
int Print2(const char* ptr, int len, int line_index)
{
    int case1 = 0, case2 = 0;

    if (len == -1)
    {
        tmp[line_index] = '\0';
        stream << tmp << std::endl;
        return 1;
    }
    else
    {
        // tmp数组在递归过程中会不断覆盖不同的字符内容
        tmp[line_index] = ptr[len];
        case1 = Print2(ptr, len -1, line_index + 1);

        case2 = Print2(ptr, len -1, line_index);
    }

    return case1 + case2;
}

void PrintTreeLeafNode(const char* ptr, int len)
{
    Print(ptr, len, g_line_index);
    stream << "elem_size: "<< g_ele_num << std::endl;

    auto cnt = Print2(ptr, len - 1, 0);
    stream << "cnt: "<< cnt << std::endl;
}