#include <iostream>
#include "stream.h"

//#include "balance_tree/balance_tree.h"
#include "balance_tree/balance_tree_debug.h"

#include "rand.h"
#include "vector.hpp"

using namespace tools;
using namespace tools::b_tree;


int main()
{
    {
        stream << "sizeof BNode: " << sizeof(_BNode<int>)<< std::endl;
        stream << "sizeof Entry: " << sizeof(_Entry<int>)<< std::endl;
        
        BalanceTree<int> my_tree(1000);

        stream << "sizeof BalanceTree is :"<< sizeof(BalanceTree<int>) << std::endl;
    }

    stream << Boolalpha;
    {
        BalanceTree<int> my_tree(3);
        for (int i = 0; i <= 10; i++)
        {
            assert(my_tree.insert(i) == true);
            assert(my_tree.is_b_tree() == true);
            // stream << "insert i: " << i << " success" << std::endl;
        }

        my_tree.print_tree1();

        int kx;
        while(std::cin>>kx, kx != -1)
        {
            stream <<my_tree.remove(kx) << std::endl;
            my_tree.print_tree1();
            stream << " is b_tree ? "<< my_tree.is_b_tree() << std::endl;
        }

        assert(my_tree.size() == 0);
    }

#if 1
    {
        BalanceTree<int> my_tree(4);
        for (int i = 0; i <= 20; i++)
        {
            assert(my_tree.insert(i) == true);
            assert(my_tree.is_b_tree() == true);
            // stream << "insert i: " << i << " success" << std::endl;
        }
        // my_tree.print_tree();
    }

    {
        BalanceTree<int> my_tree2(4);
        for (int i = 20; i >= 0; i--)
        {
            if (i == 10)
                assert(my_tree2.insert(i) == true);
            else
                assert(my_tree2.insert(i) == true);
            assert(my_tree2.is_b_tree() == true);
            // stream << "insert i: " << i << " success" << std::endl;
            //my_tree2.print_tree();
        }
        
    }

    {
        BalanceTree<int> my_tree(4);
        for (int i = 0; i < 20; i++)
        {
            assert(my_tree.insert(i) == true);
            assert(my_tree.is_b_tree() == true);
            // stream << "insert i: " << i << " success" << std::endl;
        }
        for (int i = 40; i >= 20; i--)
        {
            if (i == 30 || i == 31)
            {
                assert(my_tree.insert(i) == true);
                // my_tree.print_tree();
            }
            else
                assert(my_tree.insert(i) == true);

            if (!my_tree.is_b_tree())
            {
                my_tree.print_tree();
                assert(false);
            }
            // else
            //     stream << "insert i: " << i << " success" << std::endl;

        }
    }
#endif

#if 1
    {
        BalanceTree<int> my_tree(5);
        for (int i = 0; i <= 20; i++)
        {
            assert(my_tree.insert(i) == true);
            assert(my_tree.is_b_tree() == true);
            // stream << "insert i: " << i << " success" << std::endl;
        }
        // my_tree.print_tree();
    }

    {
        BalanceTree<int> my_tree2(5);
        for (int i = 20; i >= 0; i--)
        {
            if (i == 10)
                assert(my_tree2.insert(i) == true);
            else
                assert(my_tree2.insert(i) == true);
            assert(my_tree2.is_b_tree() == true);
            // stream << "insert i: " << i << " success" << std::endl;
            //my_tree2.print_tree();
        }
        
    }

    {
        BalanceTree<int> my_tree(5);
        for (int i = 0; i < 20; i++)
        {
            assert(my_tree.insert(i) == true);
            assert(my_tree.is_b_tree() == true);
            // stream << "insert i: " << i << " success" << std::endl;
        }
        for (int i = 40; i >= 20; i--)
        {
            if (i == 38 || i == 37)
            {
                assert(my_tree.insert(i) == true);
                //my_tree.print_tree();
            }
            else
                assert(my_tree.insert(i) == true);

            if (!my_tree.is_b_tree())
            {
                my_tree.print_tree();
                assert(false);
            }
            // else
            //     stream << "insert i: " << i << " success" << std::endl;
        }
    }
#endif

#if 1
    {
        Vector<int> array{287, 361, 202, 761, 940, 886, 371, 223, 712, 454, 771,
                          128, 459, 570, 363, 303, 963, 908, 400, 730, 309, 120,
                          236, 469, 64, 76, 252, 920, 25, 712, 140, 240, 674, 210,
                          969, 656, 745, 328, 633, 969, 237, 608, 542, 253, 140, 511,
                          575, 525, 625, 606, 948, 247, 666, 748, 218, 103, 231, 811,
                          675, 515, 586, 872, 192, 164, 274, 610, 145, 328, 658, 473,
                          941, 493, 751, 106, 760, 930, 418, 147, 696, 560, 597, 237,
                          878, 211, 873, 158, 815, 187, 438, 619, 668, 87, 718, 30, 595,
                          109, 129, 432, 969, 265};
        bool res = false;
        BalanceTree<int> my_tree(5);
        for (auto& i : array)
        {
            my_tree.insert(i);

            // if (i == 597)
            //     my_tree.print_tree();

            if (!my_tree.is_b_tree())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
                my_tree.print_tree();
                assert(false);
            }
        }
    }
#endif

#if 1
    {
        size_t ele_size = 0;
        int rang_index = 10;
        while(--rang_index > 1)
        {
            Vector<int> array;
            Rand<int> rand(1, 100000);
            for(int i = 0; i < 10000; i++)
            {
                array.push_back(rand());
            }

            // for (auto i : array)
            // {
            //     stream << i << ", ";
            // }
            // stream << std::endl;

            bool res = false;
            BalanceTree<int> my_tree(rang_index);
            for (auto& i : array)
            {
                my_tree.insert(i);

                // my_tree.print_tree();
                // stream << "insert: "<< i << std::endl;

                if (!my_tree.is_b_tree())
                {
                    stream << "insert: "<< i << " failed. ";
                    my_tree.print_tree();
                    assert(false);
                }
            }
            stream << "range_index : "<< rang_index << std::endl;
        }
        stream << "last rang_index: "<< rang_index << std::endl;
    }

#endif
    return 0;
}