#include <iostream>

#include "stream.h"

#include "priority_queue/priority_queue.hpp"
#include "rand.h"

using namespace tools;

int main()
{
    {
        PQueue<int/*, alg::greater<int>*/> my_queue;

        my_queue.push(2);
        my_queue.push(5);
        my_queue.push(8);
        my_queue.push(0);
        my_queue.push(9);
        my_queue.push(1);
        my_queue.push(3);
        my_queue.push(4);
        my_queue.push(7);
        my_queue.push(6);

        my_queue.order();

        while(!my_queue.empty())
        {
            stream << my_queue.top() << "    ";
            my_queue.pop();
            stream << ", size : "<< my_queue.size() << ",  ";
            my_queue.order();
        }
        stream << std::endl;
    }

    {
        PQueue<int> my_queue;
        int num = 100;
        int ele_size = 10000;
        while(num-- > 0)
        {
            Vector<int> array;
            Rand<int> rand(1, 100000);
            for(int i = 0; i < ele_size; i++)
            {
                array.push_back(rand());
            }

            for (auto& i : array)
            {
                my_queue.push(i);
            }

            assert(my_queue.size() == ele_size);

            {
                PQueue<int> my_queue_move = std::move(my_queue);
                assert(my_queue.empty() == true);

                PQueue<int> my_queue_copy = my_queue_move;

                my_queue = std::move(my_queue_copy);
                assert(my_queue_copy.empty() == true);
            }

            {
                PQueue<int> my_queue_move;
                my_queue_move = std::move(my_queue);
                assert(my_queue.empty() == true);

                PQueue<int> my_queue_copy;
                my_queue_copy = my_queue_move;

                my_queue = std::move(my_queue_copy);
                assert(my_queue_copy.empty() == true);
                
                // move 之后原queue彻底为空
                my_queue_copy.push(10);
                assert(my_queue_copy.size() == 1);
                assert(my_queue_copy.top() == 10);
            }

            assert(my_queue.size() == ele_size);

            int max = my_queue.top();
            int size = 0;
            while(!my_queue.empty())
            {
                assert(alg::le(max, my_queue.top()) == false);
                max = my_queue.top();
                my_queue.pop();
                size ++;
            }
            assert(size == ele_size);
            assert(my_queue.size() == 0);
        }
        stream << "pass less test" << std::endl;
    }

    {
        PQueue<int, alg::greater<int>> my_queue;
        int num = 100;
        int ele_size = 10000;
        while(num-- > 0)
        {
            Vector<int> array;
            Rand<int> rand(1, 100000);
            for(int i = 0; i < ele_size; i++)
            {
                array.push_back(rand());
            }

            for (auto& i : array)
            {
                my_queue.push(i);
            }

            assert(my_queue.size() == ele_size);

            {
                PQueue<int, alg::greater<int>> my_queue_move = std::move(my_queue);
                assert(my_queue.empty() == true);

                PQueue<int, alg::greater<int>> my_queue_copy = my_queue_move;

                my_queue = std::move(my_queue_copy);
                assert(my_queue_copy.empty() == true);
            }

            {
                PQueue<int, alg::greater<int>> my_queue_move;
                my_queue_move = std::move(my_queue);
                assert(my_queue.empty() == true);

                PQueue<int, alg::greater<int>> my_queue_copy;
                my_queue_copy = my_queue_move;

                my_queue = std::move(my_queue_copy);
                assert(my_queue_copy.empty() == true);
                
                // move 之后原queue彻底为空
                my_queue_copy.push(10);
                assert(my_queue_copy.size() == 1);
                assert(my_queue_copy.top() == 10);
            }

            assert(my_queue.size() == ele_size);

            int min = my_queue.top();
            int size = 0;
            while(!my_queue.empty())
            {
                assert(alg::le(my_queue.top(), min) == false);
                min = my_queue.top();
                my_queue.pop();
                size ++;
            }
            assert(size == ele_size);
            assert(my_queue.size() == 0);
        }
        stream << "pass greater test" << std::endl;
    }

    return 0;
}