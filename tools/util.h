#ifndef _TOOLS_UTIL_H_
#define _TOOLS_UTIL_H_

#include <iostream>
#include <cassert>
#include <cstring>
#include <string>

#include "stream.h"

namespace tools
{

extern int g_array[20];

class Util
{
public:
    template<typename T>
    static void Print(T array[], int length, const std::string& str = "")
    {
        stream << (str.empty() ? "" : (str + '\n')) << "Num: "<< length << ", Print: ";
        for (int i = 0; i < length; i++)
        {
            stream << array[i];
            if (i != length -1)
                std::cout << ", ";
        }
        stream << std::endl;
    }

    static void CopyData(void* dst, const void* src, const size_t copy_size)
    {
        if (src == g_array)
        {
            assert(copy_size == sizeof(g_array));
            memcpy(dst, src, copy_size);
            return;
        }
        assert(false);
    }
};

}

#endif /* _TOOLS_UTIL_H_ */
