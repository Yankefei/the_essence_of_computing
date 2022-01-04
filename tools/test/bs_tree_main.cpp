#include <iostream>
#include "stream.h"

#include "binary_sort_tree.h"

using namespace tools;

int main()
{
    {
        BsTree<int> my_tree;
        int ar[]={53,17,78,9,53,45,65,87,23,81,23,94,17,88};
        int n = sizeof(ar)/sizeof(ar[0]);
        stream << Boolalpha;
        for(int i = 0; i<n; ++i)
        {
            stream <<ar[i] <<" "<< my_tree.insert(ar[i]) << std::endl;
        }

        my_tree.InOrder();
        //my_tree.NiceInOrder();

        int kx;
        while(std::cin>>kx, kx != -1)
        {
            stream <<my_tree.remove(kx) << " ";
            my_tree.InOrder();
        }
    }

    return 0;
}