#include <iostream>
#include "stream.h"

//#include "binary_sort_tree.h"
#include "binary_sort_tree2.h"

using namespace tools;

int main()
{
#if 0
    {
        BsTree<int> my_tree;
        int ar[]={53,17,78,9,53,45,65,87,23,81,23,94,17,88};
        int n = sizeof(ar)/sizeof(ar[0]);
        stream << Boolalpha;
        for(int i = 0; i<n; ++i)
        {
            stream <<ar[i] <<" "<< my_tree.insert2(ar[i]) << std::endl;
        }

        my_tree.InOrder();
        //my_tree.NiceInOrder();

        stream << (my_tree.find(87) != nullptr) << std::endl;
        stream << (my_tree.find(86) != nullptr) << std::endl;

        // int kx;
        // while(std::cin>>kx, kx != -1)
        // {
        //     stream <<my_tree.remove2(kx) << " ";
        //     my_tree.InOrder();
        // }

        int ar2[] = {88, 78, 65, 53, 45};
        int n2 = sizeof(ar2)/sizeof(ar2[0]);
        for (int i = 0; i < n2; i++)
        {
            stream << my_tree.remove2(ar2[i]) << " ";
            my_tree.InOrder();
        }
    }
#endif

#if 1
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
        my_tree.NiceInOrder();

        stream << (my_tree.find(87) != nullptr) << std::endl;
        stream << (my_tree.find(86) != nullptr) << std::endl;

        int kx;
        while(std::cin>>kx, kx != -1)
        {
            stream <<my_tree.remove(kx) << " ";
            my_tree.InOrder();
        }

        // int ar2[] = {88, 78, 65, 53, 45};
        // int n2 = sizeof(ar2)/sizeof(ar2[0]);
        // for (int i = 0; i < n2; i++)
        // {
        //     stream << my_tree.remove(ar2[i]) << " ";
        //     my_tree.InOrder();
        // }
    }
#endif

    return 0;
}
