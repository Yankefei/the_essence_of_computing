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
    {
        using namespace tools::avl_tree_1;
        int rang_index = 10;
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
                my_tree.insert2(i);
                if (!my_tree.is_balance())
                {
                    stream << "insert: "<< i << " failed. ";
                }
            }
            stream << "tree hight: "<< my_tree.get_hight() << std::endl;
            for (auto& i : array)
            {
                my_tree.remove2(i);
                if (!my_tree.is_balance())
                {
                    stream << "remove: "<< i << " failed. ";
                }
            }
        }
    }
    {
        using namespace tools::avl_tree_2;
        int rang_index = 10;
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
                my_tree.insert2(i);
                if (!my_tree.is_balance())
                {
                    stream << "insert: "<< i << " failed. ";
                }
            }
            stream << "tree hight: "<< my_tree.get_hight() << std::endl;
            for (auto& i : array)
            {
                my_tree.remove2(i);
                if (!my_tree.is_balance())
                {
                    stream << "remove: "<< i << " failed. ";
                }
            }
        }
    }
    {
        using namespace tools::avl_tree_3;
        int rang_index = 10;
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
                my_tree.insert2(i);
                if (!my_tree.is_balance())
                {
                    stream << "insert: "<< i << " failed. ";
                }
            }
            stream << "tree hight: "<< my_tree.get_hight() << std::endl;
            for (auto& i : array)
            {
                my_tree.remove2(i);
                if (!my_tree.is_balance())
                {
                    stream << "remove: "<< i << " failed. ";
                }
            }
        }
    }
    return 0;
}