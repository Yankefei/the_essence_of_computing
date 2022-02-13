#include <iostream>

#include "stream.h"

#include "hash_map.hpp"
#include "rand.h"
#include "vector.hpp"
#include "string.hpp"

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
            assert(test_map.insert(i, i) == true);
        }
        assert(test_map.size() == 15);
        
        for (auto iter = test_map.begin(); iter != test_map.end(); iter ++)
        {
            stream << iter->first << "-" << iter->second << ", ";
        }
        stream << std::endl;

        for (int i = 0; i < 15; i++)
        {
            assert(test_map.find(i) != test_map.end());
        }

        for (int i = 0; i < 15; i++)
        {
            assert(test_map.insert(i, i) == false);
        }

        for (int i = 10; i < 15; i++)
        {
            assert(test_map.erase(i) == 1);
        }
        assert(test_map.size() == 10);
        for (auto iter = test_map.begin(); iter != test_map.end(); iter ++)
        {
            stream << iter->first << "-" << iter->second << ", ";
        }
        stream << std::endl;

        for (int i = 10; i < 15; i++)
        {
            assert(test_map.find(i) == test_map.end());
        }
        for (int i = 15; i < 25; i++)
        {
            assert(test_map.find(i) == test_map.end());
        }
        // for (int i = 0; i < 10; i++)
        // {
        //     assert(test_map.erase(i) == 1);
        // }

        for (auto iter = test_map.begin(); iter != test_map.end() ;)
        {
            stream << "erase : " << iter->first << std::endl;
            iter = test_map.erase(iter);
        }

        for (auto iter = test_map.begin(); iter != test_map.end(); iter ++)
        {
            stream << iter->first << "-" << iter->second << ", ";
        }
        stream << std::endl;
        assert(test_map.size() == 0);
    }

    {
        Vector<int> array{
            722, 671, 583, 326, 211, 396, 437, 910, 691, 526,
            704, 363, 664, 920, 953, 868, 490, 269, 812, 857,
            625, 966, 294, 403, 915, 867, 6, 412, 531, 229,
            170, 538, 444, 383, 354, 177, 807, 785, 182, 777,
            19, 811, 257, 533, 929, 213, 158, 522, 98, 160,
            431, 112, 393, 871, 369, 475, 910, 300, 457, 942,
            759, 295, 160, 905, 428, 10, 350, 949, 263, 700,
            135, 151, 882, 928, 415, 425, 999, 267, 579, 730,
            66, 510, 772, 701, 165, 766, 423, 509, 357, 644,
            634, 105, 487, 767, 937, 850, 202, 461, 928, 724
        }; // 910, 160, 928  重复
        HashMap<int, String> test_map;
        size_t ele_size = 0;

        for (auto& i : array)
        {
            if (test_map.insert(i, to_String(i)))
                ele_size ++;
            else
            {
                stream << "insert failed: "<< i << "  ";
            }
        }
        stream << std::endl;

        stream << "ele_size: "<< test_map.size() <<std::endl;
        for (auto iter = test_map.begin(); iter != test_map.end(); iter ++)
        {
            stream << iter->first << "-" << iter->second << ", ";
        }
        stream << std::endl;

        for (auto& i : array)
        {
            assert(test_map.find(i) != test_map.end());
        }

        for (auto& i : array)
        {
            if (test_map.erase(i) == 1)
                ele_size --;
        }

        assert(ele_size == 0);

    }

    {
        stream << "int as key" << std::endl;
        HashMap<int, int> test_map;
        size_t ele_size = 0;
        int num = 100;
        while (num -- > 0)
        {
            Vector<int> array;
            Rand<int> rand(1, 10000);
            for(int i = 0; i < 1000; i++)
            {
                array.push_back(rand());
            }

            // for (auto i : array)
            // {
            //     stream << i << ", ";
            // }
            // stream << std::endl;

            for (auto& i : array)
            {
                if (test_map.insert(i, i))
                    ele_size ++;
            }

            for (auto& i : array)
            {
                assert(test_map.find(i) != test_map.end());
            }

            // stream << "size: " << test_map.size() << std::endl;
            stream << "size: " << test_map.size() <<"  ";

            if (num % 2 == 0)
            {
                for (auto iter = test_map.begin(); iter != test_map.end() ;)
                {
                    iter = test_map.erase(iter);
                    ele_size --;
                }
            }
            else
            {
                for (auto& i : array)
                {
                    if (test_map.erase(i) == 1)
                        ele_size --;
                }
            }
            assert(test_map.size() == 0);
            assert(ele_size == 0);
        }
        stream << std::endl;
    }
    stream << "String as key... " << std::endl;

    {
        HashMap<String, int> test_map;
        size_t ele_size = 0;
        int num = 100;
        while (num -- > 0)
        {
            Vector<int> array;
            Rand<int> rand(1, 10000);
            for(int i = 0; i < 1000; i++)
            {
                array.push_back(rand());
            }

            // for (auto i : array)
            // {
            //     stream << i << ", ";
            // }
            // stream << std::endl;

            for (auto& i : array)
            {
                if (test_map.insert(to_String(i), i))
                    ele_size ++;
            }

            for (auto& i : array)
            {
                assert(test_map.find(to_String(i)) != test_map.end());
            }

            stream << "size: " << test_map.size() <<"  ";

            if (num % 2 == 0)
            {
                for (auto iter = test_map.begin(); iter != test_map.end() ;)
                {
                    iter = test_map.erase(iter);
                    ele_size --;
                }
            }
            else
            {
                for (auto& i : array)
                {
                    if (test_map.erase(to_String(i)) == 1)
                        ele_size --;
                }
            }
            assert(test_map.size() == 0);
            assert(ele_size == 0);
        }
        stream << std::endl;
    }

    return 0;
}