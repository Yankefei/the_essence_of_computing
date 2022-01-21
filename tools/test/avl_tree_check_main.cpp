#include <iostream>
#include "stream.h"

#include "binary_avl_tree.h"
#include "binary_avl_tree2.h"
#include "binary_avl_tree3.h"

#include "vector.hpp"
#include "rand.h"

using namespace tools;

int main()
{
    size_t e_size = 0;
    {
        using namespace tools::avl_tree_1;
        int rang_index = 5;
        while(rang_index -- > 0)
        {
            Vector<int> array;
            Rand<int> rand(1, 100000);
            for(int i = 0; i < 10000; i++)
            {
                array.push_back(rand());
            }

            bool res = false;
            AvlTree<int> my_tree;
            for (auto& i : array)
            {
                if (my_tree.insert(i))
                    e_size ++;
                if (!my_tree.is_balance())
                {
                    stream << "insert: "<< i << " failed. ";
                }
            }
            stream << "tree hight: "<< my_tree.get_hight() << ", size: "<< e_size << std::endl;
            for (auto& i : array)
            {
                if (my_tree.remove(i))
                    e_size --;
                if (!my_tree.is_balance())
                {
                    stream << "remove: "<< i << " failed. ";
                }
            }
            assert(e_size == 0);
        }
    }

    {
        using namespace tools::avl_tree_1;
        int rang_index = 5;
        while(rang_index -- > 0)
        {
            Vector<int> array;
            Rand<int> rand(1, 100000);
            for(int i = 0; i < 10000; i++)
            {
                array.push_back(rand());
            }

            bool res = false;
            AvlTree<int> my_tree;
            for (auto& i : array)
            {
                if (my_tree.insert2(i))
                    e_size ++;
                if (!my_tree.is_balance())
                {
                    stream << "insert: "<< i << " failed. ";
                }
            }
            stream << "tree hight: "<< my_tree.get_hight() << ", size: "<< e_size << std::endl;
            for (auto& i : array)
            {
                if (my_tree.remove2(i))
                    e_size --;
                if (!my_tree.is_balance())
                {
                    stream << "remove: "<< i << " failed. ";
                }
            }
            assert(e_size == 0);
        }
    }

    {
        using namespace tools::avl_tree_2;
        int rang_index = 5;
        while(rang_index -- > 0)
        {
            Vector<int> array;
            Rand<int> rand(1, 100000);
            for(int i = 0; i < 10000; i++)
            {
                array.push_back(rand());
            }

            bool res = false;
            AvlTree<int> my_tree;
            for (auto& i : array)
            {
                if (my_tree.insert2(i))
                    e_size ++;
                if (!my_tree.is_balance())
                {
                    stream << "insert: "<< i << " failed. ";
                }
            }
            stream << "tree hight: "<< my_tree.get_hight() << ", size: "<< e_size << std::endl;
            for (auto& i : array)
            {
                if (my_tree.remove2(i))
                    e_size --;
                if (!my_tree.is_balance())
                {
                    stream << "remove: "<< i << " failed. ";
                }
            }
            assert(e_size == 0);
        }
    }

    {
        using namespace tools::avl_tree_2;
        int rang_index = 5;
        while(rang_index -- > 0)
        {
            Vector<int> array;
            Rand<int> rand(1, 100000);
            for(int i = 0; i < 10000; i++)
            {
                array.push_back(rand());
            }

            bool res = false;
            AvlTree<int> my_tree;
            for (auto& i : array)
            {
                if (my_tree.insert(i))
                    e_size ++;
                if (!my_tree.is_balance())
                {
                    stream << "insert: "<< i << " failed. ";
                }
            }
            stream << "tree hight: "<< my_tree.get_hight() << ", size: "<< e_size << std::endl;
            for (auto& i : array)
            {
                if (my_tree.remove(i))
                    e_size --;
                if (!my_tree.is_balance())
                {
                    stream << "remove: "<< i << " failed. ";
                }
            }
            assert(e_size == 0);
        }
    }

    {
        using namespace tools::avl_tree_3;
        int rang_index = 5;
        while(rang_index -- > 0)
        {
            Vector<int> array;
            Rand<int> rand(1, 100000);
            for(int i = 0; i < 10000; i++)
            {
                array.push_back(rand());
            }

            bool res = false;
            AvlTree<int> my_tree;
            for (auto& i : array)
            {
                if (my_tree.insert2(i))
                    e_size ++;
                if (!my_tree.is_balance())
                {
                    stream << "insert: "<< i << " failed. ";
                }
            }
            stream << "tree hight: "<< my_tree.get_hight() << ", size: "<< e_size << std::endl;
            for (auto& i : array)
            {
                if (my_tree.remove2(i))
                    e_size --;
                if (!my_tree.is_balance())
                {
                    stream << "remove: "<< i << " failed. ";
                }
            }
            assert(e_size == 0);
        }
    }

    {
        using namespace tools::avl_tree_3;
        int rang_index = 5;
        while(rang_index -- > 0)
        {
            Vector<int> array;
            Rand<int> rand(1, 100000);
            for(int i = 0; i < 10000; i++)
            {
                array.push_back(rand());
            }

            bool res = false;
            AvlTree<int> my_tree;
            for (auto& i : array)
            {
                if (my_tree.insert(i))
                    e_size ++;
                if (!my_tree.is_balance())
                {
                    stream << "insert: "<< i << " failed. ";
                }
            }
            stream << "tree hight: "<< my_tree.get_hight() << ", size: "<< e_size << std::endl;
            for (auto& i : array)
            {
                if (my_tree.remove(i))
                    e_size --;
                if (!my_tree.is_balance())
                {
                    stream << "remove: "<< i << " failed. ";
                }
            }
            assert(e_size == 0);
        }
    }

    return 0;
}