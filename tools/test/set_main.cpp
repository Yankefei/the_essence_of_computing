#include <iostream>
#include "stream.h"

#include "set.hpp"
#include <set>

#include "string.hpp"
#include "rand.h"
#include "vector.hpp"

using namespace tools;

int main()
{
    // 48 byte
    stream << "sizeof std_set: "<< sizeof(std::set<int>) << std::endl;
    // 16 byte
    stream << "sizeof a_set: "<< sizeof(ASet<int>) << std::endl;
    // 24 byte
    stream << "sizeof r_set: "<< sizeof(RSet<int>) << std::endl;

    {
        ASet<int> test;
        test.insert(1);
        test.insert(1);  // 不会被覆盖，插入失败
        for (auto e : test)
        {
            stream << e <<std::endl;
        }
    }

    {
        ASet<int> c = {1, 2, 3, 4, 5, 6};
        // 从 c 擦除所有奇数
        for(auto it = c.begin(); it != c.end(); )
            if(*it % 2 == 1)
                it = c.erase(it);
            else
                ++it;
        for(auto& p : c)
            stream << p << ' ';
        stream << std::endl;
    }

    {
        ASet<int> c = {1, 2, 3, 4, 5, 6};
        
        assert(c.find(1) != c.end());
        assert(c.find(7) == c.end());

        // 从 c 擦除所有奇数
        for(auto it = c.begin(); it != c.end(); )
            if(*it % 2 == 1)
                it = c.erase(it);
            else
                ++it;
        for(auto& p : c)
            stream << p << ' ';
        stream << std::endl;
    }

    {
        ASet<int> test_set;

        Vector<int> array;
        Rand<int> rand(1, 1000);
        for(int i = 0; i < 100; i++)
        {
            array.push_back(rand());
        }

        for (auto &i : array)
        {
            test_set.insert(i);
        }

        ASet<int> test_set2 = test_set;
        test_set.clear();
        test_set = test_set2;
        test_set2.clear();
        assert(test_set2.begin() == test_set2.end());


        ASet<int> test_set3 = std::move(test_set);
        assert(test_set.begin() == test_set.end());

        assert(test_set.empty());
        test_set = std::move(test_set3);
        assert(test_set3.empty());
        assert(test_set3.begin() == test_set3.end());

        ASet<int> test_set4 = test_set;
        for (auto& i : array)
        {
            test_set4.erase(i);
        }
        assert(test_set4.empty());
        assert(test_set4.begin() == test_set4.end());


        for (auto it = test_set.begin(); it != test_set.end(); it++)
        {
            stream << *it << "  "; 
        }
        stream << std::endl;
        for (auto e : test_set)
        {
            stream << e << "  ";
        }
        stream << std::endl;
        for (auto &e : test_set)
        {
            stream << e << "  ";
        }
        stream << std::endl;
        for (const auto &e : test_set)
        {
            stream << e << "  ";
        }

        size_t set_size = test_set.size();
        
        test_set.erase(*test_set.begin());

        set_size--;
        for (auto it = test_set.begin(); it != test_set.end();)
        {
            it = test_set.erase(it);
            set_size--;
            assert(set_size == test_set.size());
        }
        stream << std::endl;
        assert(test_set.empty() == true);
    }

    {
        stream << "test ASet copy..." << std::endl;
        ASet<int> test_set;
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
                if (test_set.insert(i).second)
                    ele_size ++;
            }

            for (auto& i : array)
            {
                assert(test_set.find(i) != test_set.end());
            }

            {
                ASet<int> my_set_move = std::move(test_set);
                assert(test_set.empty() == true);

                ASet<int> my_set_copy = my_set_move;

                test_set = std::move(my_set_copy);
                assert(my_set_copy.empty() == true);
            }

            {
                ASet<int> my_set_move;
                my_set_move = std::move(test_set);
                assert(test_set.empty() == true);

                ASet<int> my_set_copy;
                my_set_copy = my_set_move;

                test_set = std::move(my_set_copy);
                assert(my_set_copy.empty() == true);
            }

            for (auto& i : array)
            {
                assert(test_set.find(i) != test_set.end());
            }

            // stream << "size: " << test_set.size() << std::endl;
            stream << "size: " << test_set.size() <<"  ";

            if (num % 2 == 0)
            {
                for (auto iter = test_set.begin(); iter != test_set.end() ;)
                {
                    iter = test_set.erase(iter);
                    ele_size --;
                }
            }
            else
            {
                for (auto& i : array)
                {
                    if (test_set.erase(i) == 1)
                        ele_size --;
                }
            }
            assert(test_set.size() == 0);
            assert(ele_size == 0);
        }
        stream << std::endl;
    }

    {
        ASet<String> my_test = {"123", "askjf", "test", "9j3", "45s", "zek"};
        for (auto it = my_test.begin(); it != my_test.end(); it ++)
            stream << *it << " ";
        stream <<my_test.size() <<std::endl;

        ASet<String> my_test2 = std::move(my_test);
        for (auto it = my_test.begin(); it != my_test.end(); it ++)
            stream << *it << " ";
        stream <<my_test.size() <<std::endl;
        for (auto it = my_test2.begin(); it != my_test2.end(); it ++)
            stream << *it << " ";
        stream <<my_test2.size() <<std::endl;
    }

////////////////////////////////////

    {
        RSet<int> test;
        test.insert(1);
        test.insert(1);  // 不会被覆盖，插入失败
        for (auto e : test)
        {
            stream << e << std::endl;
        }
    }
    
    {
        RSet<int> c = {1, 2, 3, 4, 5, 6};
        
        assert(c.find(1) != c.end());
        assert(c.find(7) == c.end());

        // 从 c 擦除所有奇数
        for(auto it = c.begin(); it != c.end(); )
            if(*it % 2 == 1)
                it = c.erase(it);
            else
                ++it;
        for(auto& p : c)
            stream << p << ' ';
        stream << std::endl;
    }

    {
        RSet<int> test_set;

        Vector<int> array;
        Rand<int> rand(1, 1000);
        for(int i = 0; i < 100; i++)
        {
            array.push_back(rand());
        }

        for (auto &i : array)
        {
            test_set.insert(i);
        }

        RSet<int> test_set2 = test_set;
        test_set.clear();
        test_set = test_set2;
        test_set2.clear();
        assert(test_set2.begin() == test_set2.end());

        RSet<int> test_set3 = std::move(test_set);
        assert(test_set.begin() == test_set.end());

        assert(test_set.empty());
        test_set = std::move(test_set3);
        assert(test_set3.empty());
        assert(test_set3.begin() == test_set3.end());


        RSet<int> test_set4 = test_set;
        for (auto& i : array)
        {
            test_set4.erase(i);
        }
        assert(test_set4.empty());
        assert(test_set4.begin() == test_set4.end());


        for (auto it = test_set.begin(); it != test_set.end(); it++)
        {
            stream << *it << "  "; 
        }
        stream << std::endl;
        for (auto e : test_set)
        {
            stream << e<< "  "; 
        }
        stream << std::endl;
        for (auto &e : test_set)
        {
            stream << e << "  "; 
        }
        stream << std::endl;
        for (const auto &e : test_set)
        {
            stream << e << "  "; 
        }

        size_t set_size = test_set.size();
        
        test_set.erase(*test_set.begin());

        set_size--;
        for (auto it = test_set.begin(); it != test_set.end();)
        {
            it = test_set.erase(it);
            set_size--;
            assert(set_size == test_set.size());
        }
        stream << std::endl;
        assert(test_set.empty() == true);
    }

    {
        stream << "test RSet copy..." << std::endl;
        RSet<int> test_set;
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
                if (test_set.insert(i).second)
                    ele_size ++;
            }

            for (auto& i : array)
            {
                assert(test_set.find(i) != test_set.end());
            }

            {
                RSet<int> my_set_move = std::move(test_set);
                assert(test_set.empty() == true);

                RSet<int> my_set_copy = my_set_move;

                test_set = std::move(my_set_copy);
                assert(my_set_copy.empty() == true);
            }

            {
                RSet<int> my_set_move;
                my_set_move = std::move(test_set);
                assert(test_set.empty() == true);

                RSet<int> my_set_copy;
                my_set_copy = my_set_move;

                test_set = std::move(my_set_copy);
                assert(my_set_copy.empty() == true);
            }

            for (auto& i : array)
            {
                assert(test_set.find(i) != test_set.end());
            }

            // stream << "size: " << test_set.size() << std::endl;
            stream << "size: " << test_set.size() <<"  ";

            if (num % 2 == 0)
            {
                for (auto iter = test_set.begin(); iter != test_set.end() ;)
                {
                    iter = test_set.erase(iter);
                    ele_size --;
                }
            }
            else
            {
                for (auto& i : array)
                {
                    if (test_set.erase(i) == 1)
                        ele_size --;
                }
            }
            assert(test_set.size() == 0);
            assert(ele_size == 0);
        }
        stream << std::endl;
    }

    {
        RSet<String> my_test = {"123", "askjf", "test", "9j3", "45s", "zek"};
        for (auto it = my_test.begin(); it != my_test.end(); it ++)
            stream << *it << " ";
        stream <<my_test.size() <<std::endl;

        RSet<String> my_test2 = std::move(my_test);
        for (auto it = my_test.begin(); it != my_test.end(); it ++)
            stream << *it << " ";
        stream <<my_test.size() <<std::endl;
        for (auto it = my_test2.begin(); it != my_test2.end(); it ++)
            stream << *it << " ";
        stream <<my_test2.size() <<std::endl;
    }
    
    return 0;
}