#include <iostream>
#include "stream.h"

#include "binary_rb_tree/binary_rb_tree.h"
#include "binary_rb_tree/binary_rb_tree_recursive.h"

#include "binary_rb_tree/binary_rb_tree2.h"
#include "binary_rb_tree/binary_rb_tree_recursive2.h"

#include "timer_guard.h"

#include "vector.hpp"
#include "rand.h"

using namespace tools;

int main()
{
    size_t ele_size = 0;

#if 1

    {
        using namespace tools::rb_tree_recurs_1;
        int rang_index = 5;
        RbTree<int> my_tree;
        while(rang_index -- > 0)
        {
            Vector<int> array;
            Rand<int> rand(1, 100000);
            for(int i = 0; i < 10000; i++)
            {
                array.push_back(rand());
            }

            TimerGuard timer("rb_tree_recurs_1");
            bool res = false;
            
            for (auto& i : array)
            {
                if (my_tree.insert(i))
                    ele_size ++;

                if (!my_tree.is_rb_tree())
                {
                    stream << "insert: "<< i << " failed. ";
                    assert(false);
                }
            }
            stream << "tree hight: "<< my_tree.get_hight() << std::endl;
            for (auto& i : array)
            {
                if (my_tree.remove(i))
                    ele_size --;

                if (!my_tree.is_rb_tree())
                {
                    stream << "remove: "<< i << " failed. ";
                    assert(false);
                }
            }
            assert(ele_size == 0);
        }
    }

#endif

#if 1

    {
        using namespace tools::rb_tree_1;
        int rang_index = 5;
        RbTree<int> my_tree;
        while(rang_index -- > 0)
        {
            Vector<int> array;
            Rand<int> rand(1, 100000);
            for(int i = 0; i < 10000; i++)
            {
                array.push_back(rand());
            }

            TimerGuard timer("rb_tree_1");
            bool res = false;
            
            for (auto& i : array)
            {
                if (my_tree.insert(i))
                    ele_size ++;

                if (!my_tree.is_rb_tree())
                {
                    stream << "insert: "<< i << " failed. ";
                    assert(false);
                }
            }
            stream << "tree hight: "<< my_tree.get_hight() << std::endl;
            for (auto& i : array)
            {
                if (my_tree.remove(i))
                    ele_size --;

                if (!my_tree.is_rb_tree())
                {
                    stream << "remove: "<< i << " failed. ";
                    assert(false);
                }
            }
            assert(ele_size == 0);
        }
    }

#endif

#if 1

    {
        using namespace tools::rb_tree_recurs_2;
        int rang_index = 5;
        RbTree<int> my_tree;
        while(rang_index -- > 0)
        {
            Vector<int> array;
            Rand<int> rand(1, 100000);
            for(int i = 0; i < 10000; i++)
            {
                array.push_back(rand());
            }

            TimerGuard timer("rb_tree_recurs_2");
            bool res = false;
            
            for (auto& i : array)
            {
                if (my_tree.insert(i))
                    ele_size ++;

                if (!my_tree.is_rb_tree())
                {
                    stream << "insert: "<< i << " failed. ";
                    assert(false);
                }
            }
            stream << "tree hight: "<< my_tree.get_hight() << std::endl;
            for (auto& i : array)
            {
                if (my_tree.remove(i))
                    ele_size --;

                if (!my_tree.is_rb_tree())
                {
                    stream << "remove: "<< i << " failed. ";
                    assert(false);
                }
            }
            assert(ele_size == 0);
        }
    }

#endif

#if 1

    {
        using namespace tools::rb_tree_2;
        int rang_index = 5;
        RbTree<int> my_tree;
        while(rang_index -- > 0)
        {
            Vector<int> array;
            Rand<int> rand(1, 100000);
            for(int i = 0; i < 10000; i++)
            {
                array.push_back(rand());
            }

            TimerGuard timer("rb_tree_2");
            bool res = false;
            
            for (auto& i : array)
            {
                if (my_tree.insert(i))
                    ele_size ++;

                if (!my_tree.is_rb_tree())
                {
                    stream << "insert: "<< i << " failed. ";
                    assert(false);
                }
            }
            stream << "tree hight: "<< my_tree.get_hight() << std::endl;
            for (auto& i : array)
            {
                if (my_tree.remove(i))
                    ele_size --;

                if (!my_tree.is_rb_tree())
                {
                    stream << "remove: "<< i << " failed. ";
                    assert(false);
                }
            }
            assert(ele_size == 0);
        }
    }

#endif

    return 0;
}