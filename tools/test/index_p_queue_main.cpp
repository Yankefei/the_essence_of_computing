#include <iostream>

#include "stream.h"

#include "priority_queue/index_priority_queue.hpp"

#include "string.hpp"

#include "vector.hpp"

using namespace tools;

// 示例用法


int main()
{
    {
        Vector<String> string_array;
        string_array.emplace_back("algorithms in c");
        string_array.emplace_back("parts 1-4");
        string_array.emplace_back("fundamentals");
        string_array.emplace_back("data structures");
        string_array.emplace_back("sorting");
        string_array.emplace_back("searching");
        string_array.emplace_back("third edition");

        struct StringCallHandle
        {
            const String& operator()(const Vector<String>* array, size_t index)
            {
                return (*array)[index];
            }
        };

        IndexPQueue<StringCallHandle, Vector<String>*>  index_p_queue(&string_array);
        IndexPQueue<StringCallHandle, Vector<String>*>  index_p_queue2 = std::move(index_p_queue);

        IndexPQueue<StringCallHandle, Vector<String>*>  index_p_queue3(&string_array);
        index_p_queue3 = std::move(index_p_queue2);


    }

    return 0;
}