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

    {
        HashMap<int, int> test_map;
        for (int i = 0; i < 15; i++)
        {
            test_map.insert(i, i);
        }
        assert(test_map.size() == 15);
        for (int i = 0; i < 15; i++)
        {
            assert(test_map.find(i) == true);
        }

        for (int i = 10; i < 15; i++)
        {
            assert(test_map.remove(i) == true);
        }
        assert(test_map.size() == 10);
        for (int i = 10; i < 15; i++)
        {
            assert(test_map.find(i) == false);
        }
        for (int i = 15; i < 25; i++)
        {
            assert(test_map.find(i) == false);
        }
        for (int i = 0; i < 10; i++)
        {
            assert(test_map.remove(i) == true);
        }
        assert(test_map.size() == 0);
    }

    {

    }

    return 0;
}