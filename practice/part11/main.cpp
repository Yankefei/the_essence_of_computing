#include <iostream>
#include <string.h>
#include "stream.h"
#include <stdio.h>

using namespace tools;

extern void get_rang_info();

extern void get_sub_array();

extern void flip();

void fgets_test()
{
    char str[100];
    fgets(str, sizeof(str), stdin);  // hello

    stream << "strlen: " << strlen(str) << std::endl; //6 (hello + 最后的换行符)
    stream << "sizeof: " << sizeof(str) << std::endl; //100
}

void get_line_test()
{
    char str[100];
    std::cin.getline(str, 100);   // hello
    stream << "strlen: " << strlen(str) << std::endl; //5  (hello)
    stream << "sizeof: " << sizeof(str) << std::endl; //100
}

// void gets_test()
// {
//     char str[100];
//     ::gets(str);  // gets()函数在C++14中被完全删除了，编译时会报错：
//     stream << "strlen: " << strlen(str) << std::endl; //5
//     stream << "sizeof: " << sizeof(str) << std::endl; //100
// }

int main()
{
    //fgets_test();
    //get_line_test();

    // get_rang_info();

    // get_sub_array();

    flip();

    return 0;
}