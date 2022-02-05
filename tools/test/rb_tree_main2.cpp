#include <iostream>
#include "stream.h"

#include "rand.h"

#include "binary_rb_tree/binary_rb_tree2.h"
//#include "binary_rb_tree/binary_rb_tree_recursive2.h"
#include "vector.hpp"

using namespace tools;
using namespace tools::rb_tree_2;
//using namespace tools::rb_tree_recurs_2;

int main()
{
    stream << Boolalpha;

#if 1
    {
        size_t ele_size = 0;
        RbTree<int> my_tree;

        stream << "sizeof RbTree<int> is "<< sizeof(RbTree<int>) << std::endl;

        Vector<int> array{10, 85, 15, 70, 20, 60, 30, 50, 65, 80, 90, 40, 5, 55, 45};
        for (auto e : array)
        {
            if (my_tree.insert(e))
                ele_size ++;
            stream << "after insert "<< e <<", is_rb_tree ? "<< my_tree.is_rb_tree() << std::endl;
            my_tree.in_order();
            
        }

        // int kx;
        // while(std::cin>>kx, kx != -1)
        // {
        //     stream <<my_tree.remove(kx) ;
        //     stream << ", after remove "<< kx <<", is_rb_tree ? "<< my_tree.is_rb_tree() << std::endl;
        //     my_tree.in_order();
        //     my_tree.print_tree();
        // }

        // my_tree.print_tree();

        while(my_tree.get_root() != nullptr)
        {
            auto kx = my_tree.get_root()->data_;
            if (my_tree.remove(kx))
                ele_size --;
            stream <<" after remove "<< kx <<", order is: "<< std::endl;
            
            my_tree.in_order();
            
            my_tree.print_tree();
            if (!my_tree.is_rb_tree())
            {
                stream << "rb_tree is invalid ." << std::endl;
                assert(false);
            }
        }
        assert(ele_size == 0);
    }

#endif

#if 1
    {
        bool res = false;
        RbTree<int> my_tree;
        for (int i = 1; i < 10; i ++)
        {
            res = my_tree.insert(i);
            if (!res || !my_tree.is_rb_tree())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
            }
        }
        
        for (int i = 30; i >19; i--)
        {
            res = my_tree.insert(i);
            if (!res || !my_tree.is_rb_tree())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
            }
        }

        for (int i = 10; i < 20; i ++)
        {
            res = my_tree.insert(i);
            if (!res || !my_tree.is_rb_tree())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
            }
        }

        // my_tree.print_tree();

        for (int i = 10; i < 20; i++)
        {
            res = my_tree.remove(i);

            // my_tree.print_tree();
            // stream << "remove i: "<< i << std::endl;
            if (!res || !my_tree.is_rb_tree())
            {
                stream << "remove: "<< i << " failed. " << std::endl;
                assert(false);
            }
        }

        for (int i = 30; i >19; i--)
        {
            res = my_tree.remove(i);

            // if (i <= 25 )
            // {
            //     stream << i << std::endl;
            //     my_tree.print_tree();
            // }

            if (!res || !my_tree.is_rb_tree())
            {
                stream << "remove: "<< i << " failed. " << std::endl;
                assert(false);
            }
        }

        for (int i = 1; i < 10; i ++)
        {
            res = my_tree.remove(i);
            {
                // my_tree.print_tree();
                // stream <<"remove: "<< i << std::endl;
            }
            if (!res || !my_tree.is_rb_tree())
            {
                stream << "remove: "<< i <<"  failed. " << std::endl;
            }
        }

        assert(my_tree.get_root() == nullptr);
    }
    stream << "pass 1 stage" << std::endl;
#endif

#if 1
    {
        bool res = false;
        RbTree<int> my_tree;
        for (int i = 1; i < 20; i ++)
        {
            res = my_tree.insert(i);
            if (!res || !my_tree.is_rb_tree())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
            }
        }
        
        for (int i = 60; i >39; i--)
        {
            res = my_tree.insert(i);
            if (!res || !my_tree.is_rb_tree())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
            }
        }

        for (int i = 20; i < 40; i ++)
        {
            res = my_tree.insert(i);
            if (!res || !my_tree.is_rb_tree())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
            }
        }


        for (int i = 20; i < 40; i++)
        {
            res = my_tree.remove(i);

            // if (i == 22 || i == 21)
            // {
            //     // my_tree.print_tree();
            // }

            if (!res || !my_tree.is_rb_tree())
            {
                stream << "remove: "<< i << " failed. " << std::endl;
                assert(false);
            }
        }

        for (int i = 60; i >39; i--)
        {
            res = my_tree.remove(i);
            if (!res || !my_tree.is_rb_tree())
            {
                stream << "remove: "<< i << " failed. " << std::endl;
                assert(false);
            }
        }

        for (int i = 1; i < 20; i ++)
        {
            res = my_tree.remove(i);
            if (!res || !my_tree.is_rb_tree())
            {
                stream << "remove: "<< i <<"  failed. " << std::endl;
                assert(false);
            }
        }

        assert(my_tree.get_root() == nullptr);

        stream << "pass 2 stage" << std::endl;
    }
#endif

#if 0
    {
        size_t ele_size = 0;
        bool res = false;
        RbTree<int> my_tree;
        for (int i = 1; i < 100; i ++)
        {
            res = my_tree.insert(i);
            if (!res || !my_tree.is_rb_tree())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
            }
            else
                ele_size ++;
        }
        
        for (int i = 200; i >149; i--)
        {
            res = my_tree.insert(i);
            if (!res || !my_tree.is_rb_tree())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
            }
            else
                ele_size ++;
        }

        for (int i = 100; i < 150; i ++)
        {
            res = my_tree.insert(i);
            if (!res || !my_tree.is_rb_tree())
            {
                stream << "insert: "<< i << " failed. " << std::endl;
            }
            else
                ele_size ++;
        }


        for (int i = 100; i < 150; i++)
        {
            if(i == 129)
            {
                res = my_tree.remove(i);
                //my_tree.print_tree();
            }
            else
                res = my_tree.remove(i);

            if (!res || !my_tree.is_rb_tree())
            {
                stream << "remove: "<< i << " failed. " << std::endl;
                assert(false);
            }
            else
                ele_size --;
        }

        for (int i = 200; i >149; i--)
        {
            res = my_tree.remove(i);
            if (!res || !my_tree.is_rb_tree())
            {
                stream << "remove: "<< i << " failed. " << std::endl;
            }
            else
                ele_size --;
        }

        for (int i = 1; i < 100; i ++)
        {
            res = my_tree.remove(i);
            if (!res || !my_tree.is_rb_tree())
            {
                stream << "remove: "<< i <<"  failed. " << std::endl;
            }
            else
                ele_size --;
        }

        assert(ele_size == 0);
        assert(my_tree.get_root() == nullptr);
        stream << "pass 3 stage" << std::endl;
    }
#endif

#if 1
    {
        size_t ele_size = 0;
        // Vector<int> array{557, 384, 351, 909, 394, 410, 384, 672, 763, 641, 773,
        //                   462, 431, 950, 979, 689, 687, 825, 581, 326, 45, 527,
        //                   100, 130, 440, 76, 270, 915, 169, 806, 362, 248, 693,
        //                   78, 494, 176, 692, 83, 439, 658, 43, 711, 549, 863,
        //                   156, 801, 797, 498, 186, 844};

        // Vector<int> array{551, 26, 902, 563, 568, 73, 820, 744, 282, 168, 915, 405,
        //                   66, 737, 238, 937, 754, 41, 201, 361, 776, 34, 746, 65,
        //                   893, 934, 168, 974, 381, 929, 78, 51, 214, 822, 234, 448,
        //                   19, 934, 32, 466, 589, 758, 589, 830, 592, 696, 212, 552, 711, 626};
        
        // Vector<int> array{119, 181, 500, 158, 516, 230, 147, 444, 514, 425, 327, 922, 921,
        //                   789, 974, 114, 705, 969, 52, 870, 551, 235, 103, 886, 995, 793,
        //                   915, 236, 920, 236, 989, 704, 515, 817, 243, 323, 807, 762, 316,
        //                   826, 62, 685, 162, 179, 865, 454, 844, 239, 365, 935};
        
        // Vector<int> array{386, 40, 488, 278, 853, 640, 876, 349, 397, 655, 899, 215, 70, 627,
        //                   754, 204, 953, 108, 403, 202, 756, 306, 556, 636, 157, 645, 905, 707,
        //                   125, 702, 269, 544, 737, 707, 873, 499, 565, 765, 572, 810, 916, 457,
        //                   989, 381, 188, 514, 193, 905, 345, 678};

        Vector<int> array{394, 202, 725, 429, 245, 455, 44, 478, 748, 112, 710, 70, 762, 559,
                          849, 333, 291, 878, 500, 804, 564, 597, 895, 508, 360, 371, 949, 351,
                          256, 996, 663, 523, 620, 610, 786, 1, 38, 449, 191, 972, 712, 25,
                          802, 118, 490, 459, 398, 289, 994, 942};
        bool res = false;
        RbTree<int> my_tree;
        for (auto& i : array)
        {
            if (my_tree.insert(i))
                ele_size ++;

            if (!my_tree.is_rb_tree())
            {
                stream << "insert: "<< i << " failed. ";
                my_tree.print_tree();
                assert(false);
            }
        }
        stream << "tree hight: "<< my_tree.get_hight() <<", size: = "<< ele_size << std::endl;
        // my_tree.print_tree();
        for (auto& i : array)
        {
            // if (i == 564 || i == 804)
            // {
            //     my_tree.print_tree();
            // }
            if (my_tree.remove(i))
                ele_size --;

            //
            // stream << "remove i : "<< i << std::endl;
            // my_tree.in_order();
            if (!my_tree.is_rb_tree())
            {
                stream << "remove: "<< i << " failed. ";
                my_tree.print_tree();
                assert(false);
            }
        }
        stream << "ele_size: "<< ele_size << std::endl;
        assert(ele_size == 0);
    }
#endif

#if 1
    {
        size_t ele_size = 0;
        int rang_index = 5;
        while(rang_index -- > 0)
        {
            Vector<int> array;
            Rand<int> rand(1, 100000);
            for(int i = 0; i < 1000; i++)
            {
                array.push_back(rand());
            }

            // for (auto i : array)
            // {
            //     stream << i << ", ";
            // }
            // stream << std::endl;

            bool res = false;
            RbTree<int> my_tree;
            for (auto& i : array)
            {
                if (my_tree.insert(i))
                    ele_size ++;

                // my_tree.print_tree();
                // stream << "insert: "<< i << std::endl;

                if (!my_tree.is_rb_tree())
                {
                    stream << "insert: "<< i << " failed. ";
                    assert(false);
                }
            }
            stream << "tree hight: "<< my_tree.get_hight()<< ", size: " << ele_size<< std::endl;
            assert(ele_size == my_tree.NiceInOrder());
            assert(ele_size == my_tree.ResNiceInOrder());
            //my_tree.print_tree();
            for (auto& i : array)
            {
                if (my_tree.remove(i))
                    ele_size --;

                // my_tree.print_tree();
                // stream << "remove i : "<< i << std::endl;

                // stream << "remove i : "<< i << std::endl;
                // my_tree.in_order();

                if (!my_tree.is_rb_tree())
                {
                    stream << "remove: "<< i << " failed. ";
                    
                    assert(false);
                }
            }
            if (ele_size != 0)
            {
                stream << "ele_size is "<< ele_size << std::endl;
                my_tree.in_order();
                my_tree.print_tree();
            }
            assert(ele_size == 0);
            assert(my_tree.get_root() == nullptr);
        }
    }
#endif


    {
        using namespace tools::rb_tree_2;
        int index = 0;
        size_t ele_size = 0;
        RbTree<int> my_tree;
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

            if (!my_tree.is_rb_tree())
            {
                assert(false);
            }

            //my_tree.in_order();
            //my_tree.print_tree();
            assert(ele_size == my_tree.NiceInOrder());
            assert(ele_size == my_tree.ResNiceInOrder());

            {
                RbTree<int> my_tree_move = std::move(my_tree);
                assert(my_tree.get_root() == nullptr);

                RbTree<int> my_tree_copy = my_tree_move;
                assert(my_tree_move.is_same(my_tree_copy) == true);

                my_tree = std::move(my_tree_copy);
                assert(my_tree.is_same(my_tree_move) == true);
                assert(my_tree_copy.get_root() == nullptr);
            }

            {
                RbTree<int> my_tree_move;
                my_tree_move = std::move(my_tree);
                assert(my_tree.get_root() == nullptr);

                RbTree<int> my_tree_copy;
                my_tree_copy = my_tree_move;
                assert(my_tree_move.is_same(my_tree_copy) == true);

                my_tree = std::move(my_tree_copy);
                assert(my_tree.is_same(my_tree_move) == true);

                assert(my_tree_copy.get_root() == nullptr);
            }

            assert(ele_size == my_tree.NiceInOrder());
            assert(ele_size == my_tree.ResNiceInOrder());

            if (!my_tree.is_rb_tree())
            {
                assert(false);
            }

            for (auto& i : array)
            {
                if (my_tree.remove(i))
                    ele_size --;
            }

            assert(my_tree.get_root() == nullptr);

            assert(ele_size == 0);
        }
    }
    return 0;
}