#include <iostream>
#include "stream.h"

#include "balance_tree_v2_debug/balance_tree_recursive_debug2.h"
//#include "balance_tree_v2_debug/balance_tree_debug2.h"

#include "rand.h"
#include "vector.hpp"

using namespace tools;
//using namespace tools::b_tree_v2_2;
using namespace tools::b_tree_recursive_v2_2;


int main()
{
    {
        stream << "sizeof BNode: " << sizeof(_BNode<int>)<< std::endl;
        stream << "sizeof Entry: " << sizeof(_Entry<int>)<< std::endl;
        
        BalanceTree<int> my_tree(1000);

        stream << "sizeof BalanceTree is :"<< sizeof(BalanceTree<int>) << std::endl;
    }

#if 0

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

        for (int i = 0; i <= 10; i++)
        {
            assert(my_tree.remove(i) == true);
            my_tree.print_tree1();
            assert(my_tree.is_b_tree() == true);
        }

        // int kx;
        // while(std::cin>>kx, kx != -1)
        // {
        //     stream <<my_tree.remove(kx) << std::endl;
        //     my_tree.print_tree1();
        //     stream << " is b_tree ? "<< my_tree.is_b_tree() << std::endl;
        // }

        assert(my_tree.size() == 0);
    }

#endif

    return 0;
}