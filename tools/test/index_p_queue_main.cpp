#include <iostream>

#include "stream.h"

#include "priority_queue/index_priority_queue.hpp"

#include "string.hpp"

#include "rand.h"
#include "vector.hpp"

using namespace tools;

// 示例用法
struct StringCallHandle
{
    const String& operator()(const Vector<String>* array, size_t index)
    {
        return (*array)[index];
    }
};

void test1()
{
    String str1("algorithms in c");
    String str2("parts 1-4");
    assert(str1 < str2);

    Vector<String> string_array;
    string_array.emplace_back("algorithms in c");  // 0
    string_array.emplace_back("parts 1-4");        // 1
    string_array.emplace_back("fundamentals");     // 2
    string_array.emplace_back("data structures");  // 3
    string_array.emplace_back("sorting");          // 4
    string_array.emplace_back("searching");        // 5
    string_array.emplace_back("third edition");    // 6
    string_array.emplace_back("yankefei");         // 7
    string_array.emplace_back("finish");           // 8
    string_array.emplace_back("index_p_queue_container");  // 9


    {
        IndexPQueue<StringCallHandle, Vector<String>*>  index_p_queue(&string_array);

        for (int i = 0; i < 10; i++)
        {
            assert(index_p_queue.push(i) == true);
            index_p_queue.order_index_array();
            assert(index_p_queue.check_index_queue() == true);
        }

        IndexPQueue<StringCallHandle, Vector<String>*>  index_p_queue2 = std::move(index_p_queue);
        IndexPQueue<StringCallHandle, Vector<String>*>  index_p_queue3(&string_array);
        index_p_queue3 = std::move(index_p_queue2);

        index_p_queue3.order_index_array();
        assert(index_p_queue3.check_index_queue() == true);

        string_array[9] = "container";  //index_p_queue_container -> container
        assert(index_p_queue3.change(9));

        stream << "after change" << std::endl;
        index_p_queue3.order_index_array();
        assert(index_p_queue3.check_index_queue() == true);

        index_p_queue2 = std::move(index_p_queue3);
        index_p_queue2.order_index_array();
        assert(index_p_queue2.check_index_queue() == true);


        size_t top_index = 0;
        while(!index_p_queue2.empty())
        {
            top_index = index_p_queue2.erase_top();
            stream << "get_top: "<< top_index <<", "<< string_array[top_index] << std::endl;
            index_p_queue2.order_index_array();
            assert(index_p_queue2.check_index_queue() == true);
        }
    }

    {
        IndexPQueue<StringCallHandle, Vector<String>*>  index_p_queue(&string_array);

        for (int i = 0; i < 10; i++)
        {
            assert(index_p_queue.push(i) == true);
            // index_p_queue.order_index_array();
            // assert(index_p_queue.check_index_queue() == true);
        }

        index_p_queue.order_index_array();
        assert(index_p_queue.check_index_queue() == true);

        for (int i = 0; i < 10; i++)
        {
            assert(index_p_queue.erase(i) == true);

            index_p_queue.order_index_array();
            assert(index_p_queue.check_index_queue() == true);

            assert(index_p_queue.erase(i) == false);
            assert(index_p_queue.data_contains(i) == false);

        }
        assert(index_p_queue.empty() == true);
    }

    {
        IndexPQueue<StringCallHandle, Vector<String>*>  index_p_queue(&string_array);

        for (int i = 0; i < 10; i++)
        {
            assert(index_p_queue.push(i) == true);
        }

        index_p_queue.order_index_array();
        assert(index_p_queue.check_index_queue() == true);

        for (int i = 9; i >= 0; i--)
        {
            assert(index_p_queue.erase(i) == true);

            index_p_queue.order_index_array();
            assert(index_p_queue.check_index_queue() == true);

            assert(index_p_queue.erase(i) == false);
            assert(index_p_queue.data_contains(i) == false);

        }
        assert(index_p_queue.empty() == true);
    }
}

void test2()
{
    String str1("algorithms in c");
    String str2("parts 1-4");
    assert(str2 > str1);

    Vector<String> string_array;
    string_array.emplace_back("algorithms in c");  // 0
    string_array.emplace_back("parts 1-4");        // 1
    string_array.emplace_back("fundamentals");     // 2
    string_array.emplace_back("data structures");  // 3
    string_array.emplace_back("sorting");          // 4
    string_array.emplace_back("searching");        // 5
    string_array.emplace_back("third edition");    // 6
    string_array.emplace_back("yankefei");         // 7
    string_array.emplace_back("finish");           // 8
    string_array.emplace_back("index_p_queue_container");  // 9


    {
        IndexPQueue<StringCallHandle, Vector<String>*, size_t, GreaterTypeCompare>
            index_p_queue(&string_array);

        for (int i = 0; i < 10; i++)
        {
            assert(index_p_queue.push(i) == true);
            index_p_queue.order_index_array();
            assert(index_p_queue.check_index_queue() == true);
        }

        IndexPQueue<StringCallHandle, Vector<String>*, size_t, GreaterTypeCompare>  index_p_queue2 = std::move(index_p_queue);
        IndexPQueue<StringCallHandle, Vector<String>*, size_t, GreaterTypeCompare>  index_p_queue3(&string_array);
        index_p_queue3 = std::move(index_p_queue2);

        index_p_queue3.order_index_array();
        assert(index_p_queue3.check_index_queue() == true);

        string_array[9] = "container";  //index_p_queue_container -> container
        assert(index_p_queue3.change(9));

        stream << "after change" << std::endl;
        index_p_queue3.order_index_array();
        assert(index_p_queue3.check_index_queue() == true);

        index_p_queue2 = std::move(index_p_queue3);
        index_p_queue2.order_index_array();
        assert(index_p_queue2.check_index_queue() == true);


        size_t top_index = 0;
        while(!index_p_queue2.empty())
        {
            top_index = index_p_queue2.erase_top();
            stream << "get_top: "<< top_index <<", "<< string_array[top_index] << std::endl;
            index_p_queue2.order_index_array();
            assert(index_p_queue2.check_index_queue() == true);
        }
    }

    {
        IndexPQueue<StringCallHandle, Vector<String>*, size_t, GreaterTypeCompare>  index_p_queue(&string_array);

        for (int i = 0; i < 10; i++)
        {
            assert(index_p_queue.push(i) == true);
            // index_p_queue.order_index_array();
            // assert(index_p_queue.check_index_queue() == true);
        }

        index_p_queue.order_index_array();
        assert(index_p_queue.check_index_queue() == true);

        for (int i = 0; i < 10; i++)
        {
            assert(index_p_queue.erase(i) == true);

            index_p_queue.order_index_array();
            assert(index_p_queue.check_index_queue() == true);

            assert(index_p_queue.erase(i) == false);
            assert(index_p_queue.data_contains(i) == false);

        }
        assert(index_p_queue.empty() == true);
    }

    {
        IndexPQueue<StringCallHandle, Vector<String>*, size_t, GreaterTypeCompare>  index_p_queue(&string_array);

        for (int i = 0; i < 10; i++)
        {
            assert(index_p_queue.push(i) == true);
        }

        index_p_queue.order_index_array();
        assert(index_p_queue.check_index_queue() == true);

        for (int i = 9; i >= 0; i--)
        {
            assert(index_p_queue.erase(i) == true);

            index_p_queue.order_index_array();
            assert(index_p_queue.check_index_queue() == true);

            assert(index_p_queue.erase(i) == false);
            assert(index_p_queue.data_contains(i) == false);

        }

        assert(index_p_queue.empty() == true);
    }
}


// 示例用法
struct IntCallHandle
{
    const int& operator()(const Vector<int>* array, size_t index)
    {
        return (*array)[index];
    }
};


void test_sample()
{
    int ele_size = 20;

    Vector<int> array = {
        222, 16, 884, 104, 740, 789, 498, 523, 525, 950,
        582, 198, 35, 571, 940, 965, 116, 922, 864, 407
    };

    IndexPQueue<IntCallHandle, Vector<int>*>  my_queue(&array);
    for (int i = 0; i < ele_size; i++)
    {
        my_queue.push(i);
    }

    assert(my_queue.size() == ele_size);

    my_queue.order_index_array();
    assert(my_queue.check_index_queue() == true);

    for (int i = 0; i < ele_size; i++)
    {
        assert(my_queue.erase(i) == true);

        my_queue.order_index_array();
        assert(my_queue.check_index_queue() == true);

        assert(my_queue.erase(i) == false);
        assert(my_queue.data_contains(i) == false);
    }
    assert(my_queue.empty() == true);
}

void test3()
{
    {
        int num = 100;
        int ele_size = 1000;
        while(num-- > 0)
        {
            Vector<int> array;
            Rand<int> rand(1, 10000);
            for(int i = 0; i < ele_size; i++)
            {
                array.push_back(rand());
            }

            // for (auto i : array)
            // {
            //     stream << i << ", ";
            // }
            // stream << std::endl;

            IndexPQueue<IntCallHandle, Vector<int>*>  my_queue(&array);
            for (int i = 0; i < ele_size; i++)
            {
                my_queue.push(i);
            }

            assert(my_queue.size() == ele_size);

            // my_queue.order_index_array();
            assert(my_queue.check_index_queue() == true);

            auto key = num % 3;
            if (key == 0)
            {
                for (int i = 0; i < ele_size; i++)
                {
                    assert(my_queue.erase(i) == true);

                    // my_queue.order_index_array();
                    assert(my_queue.check_index_queue() == true);

                    assert(my_queue.erase(i) == false);
                    assert(my_queue.data_contains(i) == false);
                }
            }
            else if (key == 1)
            {
                for (int i = ele_size - 1; i >= 0; i--)
                {
                    assert(my_queue.erase(i) == true);

                    // my_queue.order_index_array();
                    assert(my_queue.check_index_queue() == true);

                    assert(my_queue.erase(i) == false);
                    assert(my_queue.data_contains(i) == false);
                }
            }
            else
            {
                size_t top_index = my_queue.erase_top();
                assert(my_queue.check_index_queue() == true);
                int last_top_index = 0;
                while(!my_queue.empty())
                {
                    last_top_index = top_index;
                    top_index = my_queue.erase_top();
                    if (alg::le(array[last_top_index], array[top_index]))
                    {
                        assert(false);
                    }
                    assert(my_queue.check_index_queue() == true);
                }
            }
            assert(my_queue.empty() == true);
            stream << "num: " << num << std::endl;
        }
        stream << "pass less test" << std::endl;
    }
}


void test4()
{
    {
        int num = 100;
        int ele_size = 1000;
        while(num-- > 0)
        {
            Vector<int> array;
            Rand<int> rand(1, 10000);
            for(int i = 0; i < ele_size; i++)
            {
                array.push_back(rand());
            }

            IndexPQueue<IntCallHandle, Vector<int>*, size_t, GreaterTypeCompare>  my_queue(&array);
            for (int i = 0; i < ele_size; i++)
            {
                my_queue.push(i);
            }

            assert(my_queue.size() == ele_size);

            // my_queue.order_index_array();
            assert(my_queue.check_index_queue() == true);

            int key = num % 3;
            if (key == 0)
            {
                for (int i = 0; i < ele_size; i++)
                {
                    assert(my_queue.erase(i) == true);

                    // my_queue.order_index_array();
                    assert(my_queue.check_index_queue() == true);

                    assert(my_queue.erase(i) == false);
                    assert(my_queue.data_contains(i) == false);
                }
            }
            else if (key == 1)
            {
                for (int i = ele_size - 1; i >= 0; i--)
                {
                    assert(my_queue.erase(i) == true);

                    // my_queue.order_index_array();
                    assert(my_queue.check_index_queue() == true);

                    assert(my_queue.erase(i) == false);
                    assert(my_queue.data_contains(i) == false);
                }
            }
            else
            {
                size_t top_index = my_queue.erase_top();
                assert(my_queue.check_index_queue() == true);
                int last_top_index = 0;
                while(!my_queue.empty())
                {
                    last_top_index = top_index;
                    top_index = my_queue.erase_top();
                    if (alg::le(array[top_index], array[last_top_index]))
                    {
                        assert(false);
                    }
                    assert(my_queue.check_index_queue() == true);
                }
            }
            assert(my_queue.empty() == true);
            stream << "num: " << num << std::endl;
        }
        stream << "pass less test" << std::endl;
    }
}

int main()
{
#if 1
    // 最大堆测试
    stream << "begin test1 : " << std::endl;
    test1();

    // 最小堆测试
    stream << "begin test2 : " << std::endl;
    test2();

#endif

    test_sample();

    // 最大堆随机数测试
    test3();

    // 最小堆随机数测试
    test4();

    return 0;
}
