#include "stream.h"

#include "balance_plus_tree.h"


using namespace tools;
using namespace tools::b_plus_tree;


int main()
{

    stream << "sizeof BNode: " << sizeof(_BNode<int>)<< std::endl;

    stream << "sizeof Entry: " << sizeof(Entry<int>)<< std::endl;
    

    {
        BalancePlusTree<int> my_tree(10);

        stream << "sizeof BalancePlusTree is :"<< sizeof(BalancePlusTree<int>) << std::endl;
    
        auto ptr = my_tree.get_root();
        ptr->m_ = 10;
        for (int i = 0; i < 10; i ++)
        {
            ptr->array_[i].data_ = i;
        }

        for (int i = 0; i < 10; i ++)
        {
            stream << ptr->array_[i].data_<<" ";
        }
        stream << std::endl;
    }

    return 0;
}