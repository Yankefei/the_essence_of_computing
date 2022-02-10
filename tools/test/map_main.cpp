#include "stream.h"

#include "map.hpp"
#include "string.hpp"
#include <map>

#include "rand.h"
#include "vector.hpp"

using namespace tools;

int main()
{

    stream << "sizeof std_map: "<< sizeof(std::map<int, int>) << std::endl;
    
    stream << "sizeof r_map: "<< sizeof(RMap<int, int>) << std::endl;

    {
        std::map<int, int> test;
        test.insert(std::pair<int, int>{1, 2});
        test.insert(std::pair<int, int>{1, 3});  // 不会被覆盖，插入失败
        for (auto e : test)
        {
            stream << e.first << ", "<< e.second << std::endl;
        }
    }

    {
        AMap<int, int> a_test_map; 
    }

    {
        std::map<int, String> c = {{1, "one"}, {2, "two"}, {3, "three"},
                                        {4, "four"}, {5, "five"}, {6, "six"}};
        // 从 c 擦除所有奇数
        for(auto it = c.begin(); it != c.end(); )
            if(it->first % 2 == 1)
                it = c.erase(it);
            else
                ++it;
        for(auto& p : c)
            stream << p.second << ' ';
        stream << std::endl;
    }

    {
        AMap<int, String> c = {{1, "one"}, {2, "two"}, {3, "three"},
                                        {4, "four"}, {5, "five"}, {6, "six"}};
        // 从 c 擦除所有奇数
        for(auto it = c.begin(); it != c.end(); )
            if(it->first % 2 == 1)
                it = c.erase(it);
            else
                ++it;
        for(auto& p : c)
            stream << p.second << ' ';
        stream << std::endl;
    }

    {
        AMap<int, String> test_map;

        Vector<int> array;
        Rand<int> rand(1, 1000);
        for(int i = 0; i < 100; i++)
        {
            array.push_back(rand());
        }

        for (auto &i : array)
        {
            test_map.insert(Pair<int, String>(i, to_String(i)));
        }

        for (auto it = test_map.begin(); it != test_map.end(); it++)
        {
            stream << it->first << "-"<< it->second << "  "; 
        }
        size_t map_size = test_map.size();
        
        test_map.erase((*test_map.begin()).first);

        map_size--;
        for (auto it = test_map.begin(); it != test_map.end();)
        {
            it = test_map.erase(it);
            map_size--;
            assert(map_size == test_map.size());
        }
        stream << std::endl;
    }

////////////////////////////////////

    {
        RMap<int, int> r_test_map;
    }
    
    {
        RMap<int, String> c = {{1, "one"}, {2, "two"}, {3, "three"},
                                        {4, "four"}, {5, "five"}, {6, "six"}};
        // 从 c 擦除所有奇数
        for(auto it = c.begin(); it != c.end(); )
            if(it->first % 2 == 1)
                it = c.erase(it);
            else
                ++it;
        for(auto& p : c)
            stream << p.second << ' ';
        stream << std::endl;
    }

    {
        RMap<int, String> test_map;

        Vector<int> array;
        Rand<int> rand(1, 1000);
        for(int i = 0; i < 100; i++)
        {
            array.push_back(rand());
        }

        for (auto &i : array)
        {
            test_map.insert(Pair<int, String>(i, to_String(i)));
        }

        for (auto it = test_map.begin(); it != test_map.end(); it++)
        {
            stream << it->first << "-"<< it->second << "  "; 
        }
        size_t map_size = test_map.size();
        
        test_map.erase((*test_map.begin()).first);

        map_size--;
        for (auto it = test_map.begin(); it != test_map.end();)
        {
            it = test_map.erase(it);
            map_size--;
            assert(map_size == test_map.size());
        }
        stream << std::endl;
    }

    return 0;
}