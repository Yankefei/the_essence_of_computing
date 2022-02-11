#include "stream.h"

#include "map.hpp"
#include "string.hpp"
#include <map>

#include "rand.h"
#include "vector.hpp"

using namespace tools;

int main()
{

    // 48 byte
    stream << "sizeof std_map: "<< sizeof(std::map<int, int>) << std::endl;
    // 16 byte
    stream << "sizeof a_map: "<< sizeof(AMap<int, int>) << std::endl;
    // 24 byte
    stream << "sizeof r_map: "<< sizeof(RMap<int, int>) << std::endl;

    {
        AMap<int, int> test;
        test.insert(Pair<int, int>{1, 2});
        test.insert(Pair<int, int>{1, 3});  // 不会被覆盖，插入失败
        for (auto e : test)
        {
            stream << e.first << ", "<< e.second << std::endl;
        }
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
        
        assert(c.find(1) != c.end());
        assert(c.find(7) == c.end());

        c[2] = "two_two";
        c[7] = "seven";
        c[8] = "eight";

        assert(c.find(7) != c.end());

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

        AMap<int, String> test_map2 = test_map;
        test_map.clear();
        test_map = test_map2;
        test_map2.clear();
        assert(test_map2.begin() == test_map2.end());


        AMap<int, String> test_map3 = std::move(test_map);
        assert(test_map.begin() == test_map.end());

        assert(test_map.empty());
        test_map = std::move(test_map3);
        assert(test_map3.empty());
        assert(test_map3.begin() == test_map3.end());

        AMap<int, String> test_map4 = test_map;
        for (auto& i : array)
        {
            test_map4.erase(i);
        }
        assert(test_map4.empty());
        assert(test_map4.begin() == test_map4.end());


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
        assert(test_map.empty() == true);
    }

////////////////////////////////////

    {
        RMap<int, int> test;
        test.insert(Pair<int, int>{1, 2});
        test.insert(Pair<int, int>{1, 3});  // 不会被覆盖，插入失败
        for (auto e : test)
        {
            stream << e.first << ", "<< e.second << std::endl;
        }
    }
    
    {
        RMap<int, String> c = {{1, "one"}, {2, "two"}, {3, "three"},
                                        {4, "four"}, {5, "five"}, {6, "six"}};
        
        assert(c.find(1) != c.end());
        assert(c.find(7) == c.end());

        c[2] = "two_two";
        c[7] = "seven";
        c[8] = "eight";

        assert(c.find(7) != c.end());

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

        RMap<int, String> test_map2 = test_map;
        test_map.clear();
        test_map = test_map2;
        test_map2.clear();
        assert(test_map2.begin() == test_map2.end());

        RMap<int, String> test_map3 = std::move(test_map);
        assert(test_map.begin() == test_map.end());

        assert(test_map.empty());
        test_map = std::move(test_map3);
        assert(test_map3.empty());
        assert(test_map3.begin() == test_map3.end());


        RMap<int, String> test_map4 = test_map;
        for (auto& i : array)
        {
            test_map4.erase(i);
        }
        assert(test_map4.empty());
        assert(test_map4.begin() == test_map4.end());


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
        assert(test_map.empty() == true);
    }

    return 0;
}