#include "stream.h"

#include "balance_tree.h"


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

    {
        BalanceTree<int> my_tree(4);
        for (int i = 0; i <= 20; i++)
        {
            assert(my_tree.insert(i) == true);
            assert(my_tree.is_b_tree() == true);
            stream << "insert i: " << i << " success" << std::endl;
        }
        my_tree.print_tree();
    }

    {
        BalanceTree<int> my_tree2(4);
        for (int i = 20; i >= 0; i--)
        {
            assert(my_tree2.insert(i) == true);
            assert(my_tree2.is_b_tree() == true);
            stream << "insert i: " << i << " success" << std::endl;
        }
        my_tree2.print_tree();
    }

    {
        BalanceTree<int> my_tree(4);
        for (int i = 0; i < 20; i++)
        {
            assert(my_tree.insert(i) == true);
            assert(my_tree.is_b_tree() == true);
            stream << "insert i: " << i << " success" << std::endl;
        }
        for (int i = 40; i >= 20; i--)
        {
            assert(my_tree.insert(i) == true);
            assert(my_tree.is_b_tree() == true);
            stream << "insert i: " << i << " success" << std::endl;
            my_tree.print_tree();
        }
    }

    return 0;
}