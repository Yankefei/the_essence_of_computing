#include "stream.h"

#include "balance_tree/balance_plus_tree.h"


using namespace tools;
using namespace tools::b_plus_tree;


int main()
{

    stream << "sizeof BNode: " << sizeof(_BNode<int>)<< std::endl;

    stream << "sizeof Entry: " << sizeof(_Entry<int>)<< std::endl;
    

    {
        BalancePlusTree<int> my_tree(10);

        stream << "sizeof BalancePlusTree is :"<< sizeof(BalancePlusTree<int>) << std::endl;
    

    }

    return 0;
}