#include <iostream>
#include "stream.h"

#include "binary_avl_tree/binary_avl_tree3.h"

#include "vector.hpp"
#include "rand.h"

using namespace tools;
using namespace tools::avl_tree_3;

int main()
{
    stream << Boolalpha;
#if 1
    {
        AvlTree<int> my_tree;
        stream <<"sizeof AvlTree:" << sizeof(AvlTree<int>) << std::endl;

        for (int i = 1; i < 8; i++)
        {
            my_tree.insert2(i);
            stream <<"insert: "<< i <<", root: " << my_tree.get_root()->data_ << " "
               /*<< my_tree.get_root()->hight_*/ <<" "<<my_tree.get_hight() << " "
               << my_tree.is_balance() <<std::endl;
            // my_tree.print_tree();
        }
        for (int i = 16; i > 7; i--)
        {
            my_tree.insert2(i);
            stream <<"insert: "<< i <<", root: " << my_tree.get_root()->data_ << " "
               /*<< my_tree.get_root()->hight_*/ <<" "<< my_tree.get_hight() << " "
               << my_tree.is_balance() << std::endl;
            // my_tree.print_tree();
        }

        my_tree.InOrder();
        my_tree.NiceInOrder();
        my_tree.ResNiceInOrder();
        stream <<"root: " << my_tree.get_root()->data_ << " "
               /*<< my_tree.get_root()->hight_*/ <<" "<< my_tree.get_hight() << " "
               << my_tree.is_balance() << std::endl;

        my_tree.print_tree();

        for (int i = 1; i < 17; i++)
        {
            my_tree.remove(i);
        }
        assert(my_tree.get_root() == nullptr);

        // int kx;
        // while(std::cin>>kx, kx != -1)
        // {
        //     stream <<my_tree.remove(kx) ;
        //     stream << " is bal? "<< my_tree.is_balance() << std::endl;
        //     my_tree.InOrder();
        //     my_tree.print_tree();
        // }
    }
#endif

#if 1
    {
        bool res = false;
        AvlTree<int> my_tree;
        for (int i = 1; i < 10; i ++)
        {
            res = my_tree.insert(i);
            if (!res || !my_tree.is_balance())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
            }
        }
        
        for (int i = 30; i >19; i--)
        {
            res = my_tree.insert(i);
            if (!res || !my_tree.is_balance())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
            }
        }

        for (int i = 10; i < 20; i ++)
        {
            res = my_tree.insert(i);
            if (!res || !my_tree.is_balance())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
            }
        }


        for (int i = 10; i < 20; i++)
        {
            res = my_tree.remove(i);

            if (!res || !my_tree.is_balance())
            {
                stream << "remove: "<< i << " failed. " << std::endl;
            }
        }

        for (int i = 30; i >19; i--)
        {
            res = my_tree.remove(i);

            //if (i <= 28)
            // {
            //     stream << i << std::endl;
            //     my_tree.print_tree();
            // }

            if (!res || !my_tree.is_balance())
            {
                stream << "remove: "<< i << " failed. " << std::endl;
            }
        }

        for (int i = 1; i < 10; i ++)
        {
            res = my_tree.remove(i);
            // {
            //     stream << i << std::endl;
            //     my_tree.print_tree();
            // }
            if (!res || !my_tree.is_balance())
            {
                stream << "remove: "<< i <<"  failed. " << std::endl;
            }
        }

        assert(my_tree.get_root() == nullptr);
    }

#endif

#if 1
    {
        bool res = false;
        AvlTree<int> my_tree;
        for (int i = 1; i < 20; i ++)
        {
            res = my_tree.insert(i);
            if (!res || !my_tree.is_balance())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
            }
        }
        
        for (int i = 60; i >39; i--)
        {
            res = my_tree.insert(i);
            if (!res || !my_tree.is_balance())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
            }
        }

        for (int i = 20; i < 40; i ++)
        {
            res = my_tree.insert(i);
            if (!res || !my_tree.is_balance())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
            }
        }


        for (int i = 20; i < 40; i++)
        {
            res = my_tree.remove(i);

            if (i == 34)
            {
                my_tree.print_tree();
            }

            if (!res || !my_tree.is_balance())
            {
                stream << "remove: "<< i << " failed. " << std::endl;
            }
        }

        for (int i = 60; i >39; i--)
        {
            res = my_tree.remove(i);
            if (!res || !my_tree.is_balance())
            {
                stream << "remove: "<< i << " failed. " << std::endl;
            }
        }

        for (int i = 1; i < 20; i ++)
        {
            res = my_tree.remove(i);
            if (!res || !my_tree.is_balance())
            {
                stream << "remove: "<< i <<"  failed. " << std::endl;
            }
        }

        assert(my_tree.get_root() == nullptr);
    }
#endif

#if 1
    {
        bool res = false;
        AvlTree<int> my_tree;
        for (int i = 1; i < 100; i ++)
        {
            res = my_tree.insert(i);
            if (!res || !my_tree.is_balance())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
            }
        }
        
        for (int i = 200; i >149; i--)
        {
            res = my_tree.insert(i);
            if (!res || !my_tree.is_balance())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
            }
        }

        for (int i = 100; i < 150; i ++)
        {
            res = my_tree.insert(i);
            if (!res || !my_tree.is_balance())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
            }
        }





        for (int i = 100; i < 150; i++)
        {
            res = my_tree.remove(i);
            if (!res || !my_tree.is_balance())
            {
                stream << "remove: "<< i << " failed. " << std::endl;
            }
        }

        for (int i = 200; i >149; i--)
        {
            res = my_tree.remove(i);
            if (!res || !my_tree.is_balance())
            {
                stream << "remove: "<< i << " failed. " << std::endl;
            }
        }

        for (int i = 1; i < 100; i ++)
        {
            res = my_tree.remove(i);
            if (!res || !my_tree.is_balance())
            {
                stream << "remove: "<< i <<"  failed. " << std::endl;
            }
        }

        assert(my_tree.get_root() == nullptr);
    }
#endif

#if 1
    {
        bool res = false;
        AvlTree<int> my_tree;
        for (int i = 1; i < 100; i ++)
        {
            res = my_tree.insert(i);
            if (!res || !my_tree.is_balance())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
            }
        }
        
        for (int i = 200; i >149; i--)
        {
            res = my_tree.insert(i);
            if (!res || !my_tree.is_balance())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
            }
        }

        for (int i = 100; i < 150; i ++)
        {
            res = my_tree.insert(i);
            if (!res || !my_tree.is_balance())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
            }
        }

        while(my_tree.get_root() != nullptr)
        {
            my_tree.remove(my_tree.get_root()->data_);
        }
    }
#endif

    {
        Vector<int> array;
        Rand<int> rand(1, 10000);
        for(int i = 0; i < 1000; i++)
        {
            array.push_back(rand());
        }

        bool res = false;
        AvlTree<int> my_tree;
        for (auto& i : array)
        {
            my_tree.insert(i);
            if (!my_tree.is_balance())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
                // my_tree.print_tree();
                // assert(false);
            }
        }
        stream << "tree hight: "<< my_tree.get_hight() << std::endl;
        my_tree.NiceInOrder();
        my_tree.ResNiceInOrder();
        for (auto& i : array)
        {
            my_tree.remove(i);
            if (!my_tree.is_balance())
            {
                stream << "remove: "<< i << " failed. " << std::endl;
            }
        }
    }

    {
        using namespace tools::avl_tree_3;
        AvlTree<int> my_tree;
        int index = 0;
        while(index ++ < 10)
        {
            Vector<int> array;
            Rand<int> rand(1, 10000);
            for(int i = 0; i < 1000; i++)
            {
                array.push_back(rand());
            }

            bool res = false;
            size_t ele_size = 0;
            for (auto& i : array)
            {
                if (my_tree.insert2(i))
                    ele_size++;
                if (!my_tree.is_balance())
                {
                    stream << "insert: "<< i << " failed. " << std::endl;
                    // my_tree.print_tree();
                    // assert(false);
                }
            }
            stream << "tree hight: "<< my_tree.get_hight() << std::endl;
            assert(ele_size == my_tree.NiceInOrder());
            assert(ele_size == my_tree.ResNiceInOrder());
            for (auto& i : array)
            {
                if (my_tree.remove2(i))
                    ele_size--;
                if (!my_tree.is_balance())
                {
                    stream << "remove: "<< i << " failed. " << std::endl;
                }
            }

            assert(ele_size == 0);
        }
    }

    {
        using namespace tools::avl_tree_3;
        stream << "check copy func" << std::endl;
        int index = 0;
        size_t ele_size = 0;
        AvlTree<int> my_tree;
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

            if (!my_tree.is_balance())
            {
                assert(false);
            }

            //my_tree.in_order();
            //my_tree.print_tree();
            assert(ele_size == my_tree.NiceInOrder());
            assert(ele_size == my_tree.ResNiceInOrder());

            {
                AvlTree<int> my_tree_move = std::move(my_tree);
                assert(my_tree.get_root() == nullptr);

                AvlTree<int> my_tree_copy = my_tree_move;
                assert(my_tree_move.is_same(my_tree_copy) == true);

                my_tree = std::move(my_tree_copy);
                assert(my_tree.is_same(my_tree_move) == true);
                assert(my_tree_copy.get_root() == nullptr);
            }

            {
                AvlTree<int> my_tree_move;
                my_tree_move = std::move(my_tree);
                assert(my_tree.get_root() == nullptr);

                AvlTree<int> my_tree_copy;
                my_tree_copy = my_tree_move;
                assert(my_tree_move.is_same(my_tree_copy) == true);

                my_tree = std::move(my_tree_copy);
                assert(my_tree.is_same(my_tree_move) == true);

                assert(my_tree_copy.get_root() == nullptr);
            }

            assert(ele_size == my_tree.NiceInOrder());
            assert(ele_size == my_tree.ResNiceInOrder());

            if (!my_tree.is_balance())
            {
                assert(false);
            }

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