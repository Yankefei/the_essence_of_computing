#include <iostream>

#include <cstring>
#include "stream.h"
#include "vector.hpp"
#include "general.h"

using namespace tools;
using namespace std;

struct Struct1
{
    char c;
    int i;
}ALIGN(8);   // 8


struct Struct2
{
    short a[2];
}ALIGN(8);   // 8

struct Struct3
{
    uint32_t in_ CACHE_LINE_ALIGN;
    uint32_t out_ CACHE_LINE_ALIGN;
};  // 128

// 这种在内部填充一个废弃的pad字段的方式将cache line隔开的方式更有通用性
// struct Struct4_这种的话，虽然做到了再结构体内部将字段按照cache line隔开，但是如果
// cpu加载数据的时候，没有按照cache line对齐的方式加载，实际结果还会有影响。
struct Struct4
{
    uint32_t in_;
    //-------
    EMPTY_CACHE_LINE;
    uint32_t out_;
    uint32_t out2_;
    uint32_t out3_;
    //---------
    EMPTY_CACHE_LINE;
    uint32_t out4_;
    //----------

};  // 192 = 64 * 3

struct Struct4_
{
    uint32_t in_;
    //-------

    uint32_t out_ CACHE_LINE_ALIGN;
    uint32_t out2_;
    uint32_t out3_;
    //---------

    uint32_t out4_ CACHE_LINE_ALIGN;
    //----------

};  // 192 = 64 * 3

struct Struct5
{
    uint32_t in_ CACHE_LINE_ALIGN;
    uint32_t out_;
};  // 64

struct Struct6
{
    uint32_t in_ ;
    uint32_t out_ CACHE_LINE_ALIGN;
};  // 128

struct Struct7
{
    uint32_t in_;
    EMPTY_CACHE_LINE;

    uint32_t out_;
    EMPTY_CACHE_LINE;

    char* buf_;
};  // 192 = 64 * 3

int main()
{
    stream <<"1 " << sizeof(Struct1) << std::endl;
    stream <<"2 "<< sizeof(Struct2) << std::endl;

    stream <<"3 "<< sizeof(Struct3) << std::endl;
    stream <<"4 "<< sizeof(Struct4) << std::endl;
    stream <<"4_ "<< sizeof(Struct4_) << std::endl;
    stream <<"5 "<< sizeof(Struct5) << std::endl;
    stream <<"6 "<< sizeof(Struct6) << std::endl;
    stream <<"7 "<< sizeof(Struct7) << std::endl;
    return 0;
}