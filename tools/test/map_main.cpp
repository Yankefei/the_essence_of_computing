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
        stream << std::endl;
        for (auto e : test_map)
        {
            stream << e.first << "-"<< e.second << "  "; 
        }
        stream << std::endl;
        for (auto &e : test_map)
        {
            stream << e.first << "-"<< e.second << "  "; 
        }
        stream << std::endl;
        for (const auto &e : test_map)
        {
            stream << e.first << "-"<< e.second << "  "; 
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

    {
        stream << "test AMap copy..." << std::endl;
        AMap<int, String> test_map;
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

            for (auto& i : array)
            {
                if (test_map.insert(Pair<int, String>(i, to_String(i))).second)
                    ele_size ++;
            }

            for (auto& i : array)
            {
                assert(test_map.find(i) != test_map.end());
            }

            {
                AMap<int, String> my_map_move = std::move(test_map);
                assert(test_map.empty() == true);

                AMap<int, String> my_map_copy = my_map_move;

                test_map = std::move(my_map_copy);
                assert(my_map_copy.empty() == true);
            }

            {
                AMap<int, String> my_map_move;
                my_map_move = std::move(test_map);
                assert(test_map.empty() == true);

                AMap<int, String> my_map_copy;
                my_map_copy = my_map_move;

                test_map = std::move(my_map_copy);
                assert(my_map_copy.empty() == true);
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
        stream << std::endl;
        for (auto e : test_map)
        {
            stream << e.first << "-"<< e.second << "  "; 
        }
        stream << std::endl;
        for (auto &e : test_map)
        {
            stream << e.first << "-"<< e.second << "  "; 
        }
        stream << std::endl;
        for (const auto &e : test_map)
        {
            stream << e.first << "-"<< e.second << "  "; 
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

    {
        stream << "test RMap copy..." << std::endl;
        RMap<int, String> test_map;
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

            for (auto& i : array)
            {
                if (test_map.insert(Pair<int, String>(i, to_String(i))).second)
                    ele_size ++;
            }

            for (auto& i : array)
            {
                assert(test_map.find(i) != test_map.end());
            }

            {
                RMap<int, String> my_map_move = std::move(test_map);
                assert(test_map.empty() == true);

                RMap<int, String> my_map_copy = my_map_move;

                test_map = std::move(my_map_copy);
                assert(my_map_copy.empty() == true);
            }

            {
                RMap<int, String> my_map_move;
                my_map_move = std::move(test_map);
                assert(test_map.empty() == true);

                RMap<int, String> my_map_copy;
                my_map_copy = my_map_move;

                test_map = std::move(my_map_copy);
                assert(my_map_copy.empty() == true);
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

    return 0;
}