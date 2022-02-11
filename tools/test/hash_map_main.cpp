#include <iostream>

#include "stream.h"

#include "hash_map.hpp"

#include <unordered_map>

using namespace tools;

int main()
{
    {
        stream << "std::unordered_map size : " << sizeof(std::unordered_map<int, int>) << std::endl;
    }

    {
        HashMap<int, int> test_map;
        stream << "HashMap size : " << sizeof(HashMap<int, int>) << std::endl;
    }

    return 0;
}