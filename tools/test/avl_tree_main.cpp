#include <iostream>
#include "stream.h"

#include "binary_avl_tree.h"

using namespace tools;

int main()
{
    {
        AvlTree<int> my_tree;
        stream <<"sizeof AvlTree:" << sizeof(AvlTree<int>) << std::endl;

        stream << Boolalpha;
        for (int i = 1; i < 8; i++)
        {
            my_tree.insert(i);
            stream <<"insert: "<< i <<", root: " << my_tree.get_root()->data_ << " "
               << my_tree.get_root()->hight_ <<" "<<my_tree.get_hight() << " "
               << my_tree.is_balance() <<std::endl;
        }
        for (int i = 16; i > 7; i--)
        {
            my_tree.insert(i);
            stream <<"insert: "<< i <<", root: " << my_tree.get_root()->data_ << " "
               << my_tree.get_root()->hight_ <<" "<< my_tree.get_hight() << " "
               << my_tree.is_balance() << std::endl;
        }

        my_tree.InOrder();
        stream <<"root: " << my_tree.get_root()->data_ << " "
               << my_tree.get_root()->hight_ <<" "<< my_tree.get_hight() << " "
               << my_tree.is_balance() << std::endl;

        my_tree.print_tree();

        int kx;
        while(std::cin>>kx, kx != -1)
        {
            stream <<my_tree.remove(kx) << " is bal? "<< my_tree.is_balance() << std::endl;
            my_tree.InOrder();
            my_tree.print_tree();
        }
    }

    {

    }

    return 0;
}