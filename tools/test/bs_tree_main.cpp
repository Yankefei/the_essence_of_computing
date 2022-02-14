#include <iostream>
#include "stream.h"

//#include "binary_sort_tree.h"
//#include "binary_sort_tree2.h"
#include "binary_sort_tree3.h"

#include "vector.hpp"
#include "rand.h"

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

#if 0
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

#if 1
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
        my_tree.NiceInOrder();
        my_tree.ResNiceInOrder();

        stream << (my_tree.find(87) != nullptr) << std::endl;
        stream << (my_tree.find(86) != nullptr) << std::endl;

        // int kx;
        // while(std::cin>>kx, kx != -1)
        // {
        //     stream <<my_tree.remove(kx) << " ";
        //     my_tree.NiceInOrder();
        //     my_tree.ResNiceInOrder();
        // }

        while(my_tree.get_root() != nullptr)
        {
            stream <<my_tree.remove(my_tree.get_root()->data_) << " ";
            my_tree.NiceInOrder();
            my_tree.ResNiceInOrder();
        }
        stream << std::endl;

        // int ar2[] = {88, 78, 65, 53, 45};
        // int n2 = sizeof(ar2)/sizeof(ar2[0]);
        // for (int i = 0; i < n2; i++)
        // {
        //     stream << my_tree.remove(ar2[i]) << " ";
        //     my_tree.InOrder();
        // }
    }
#endif

    {
        stream << "check copy func" << std::endl;
        int index = 0;
        size_t ele_size = 0;
        BsTree<int> my_tree;
        while(index ++ < 10)
        {
            Vector<int> array;
            Rand<int> rand(1, 1000000);
            for(int i = 0; i < 10000; i++)
            {
                array.push_back(rand());
            }
            
            for (auto& i : array)
            {
                if (my_tree.insert(i))
                    ele_size ++;
            }

            //my_tree.in_order();
            //my_tree.print_tree();
            assert(ele_size == my_tree.NiceInOrder());
            assert(ele_size == my_tree.ResNiceInOrder());

            {
                BsTree<int> my_tree_move = std::move(my_tree);
                assert(my_tree.get_root() == nullptr);

                BsTree<int> my_tree_copy = my_tree_move;
                assert(my_tree_move.is_same(my_tree_copy) == true);

                my_tree = std::move(my_tree_copy);
                assert(my_tree.is_same(my_tree_move) == true);
                assert(my_tree_copy.get_root() == nullptr);
            }

            {
                BsTree<int> my_tree_move;
                my_tree_move = std::move(my_tree);
                assert(my_tree.get_root() == nullptr);

                BsTree<int> my_tree_copy;
                my_tree_copy = my_tree_move;
                assert(my_tree_move.is_same(my_tree_copy) == true);

                my_tree = std::move(my_tree_copy);
                assert(my_tree.is_same(my_tree_move) == true);

                assert(my_tree_copy.get_root() == nullptr);
            }

            assert(ele_size == my_tree.NiceInOrder());
            assert(ele_size == my_tree.ResNiceInOrder());

            for (auto& i : array)
            {
                if (/*my_tree.remove(i) && */my_tree.remove(i))
                    ele_size --;
            }
            //assert(my_tree.get_root() == nullptr);
            assert(my_tree.get_root() == nullptr);

            assert(ele_size == 0);
        }
    }

    return 0;
}
