#include <iostream>
#include "stream.h"

#include <cassert>
#include "rand.h"

#include "balance_tree/balance_tree.h"
#include "balance_tree/balance_tree2.h"

#include  "balance_plus_tree/balance_plus_tree.h"
#include  "balance_plus_tree/balance_plus_tree2.h"

using namespace tools;


int main()
{
#if 1
    {
        using namespace tools::b_tree;
        stream << "tools::b_tree" << std::endl;
        size_t ele_size = 0;
        int rang_index = 10;
        while(--rang_index > 1)
        {
            BalanceTree<int> my_tree(rang_index);
            int num = 5;
            while (num -- > 0)
            {
                Vector<int> array;
                Rand<int> rand(1, 10000);
                for(int i = 0; i < 1000; i++)
                {
                    array.push_back(rand());
                }

                // for (auto i : array)
                // {
                //     stream << i << ", ";
                // }
                // stream << std::endl;
                
                for (auto& i : array)
                {
                    if (my_tree.insert(i))
                        ele_size ++;

                    // my_tree.print_tree();
                    // stream << "insert: "<< i << std::endl;

                    if (!my_tree.is_b_tree())
                    {
                        stream << "insert: "<< i << " failed. " << std::endl;
                        my_tree.print_tree();
                        assert(false);
                    }
                }

                for (auto& i : array)
                {
                    assert(my_tree.search(i) == true);
                }

                {
                    BalanceTree<int> my_tree_move = std::move(my_tree);
                    assert(my_tree.get_root() == nullptr);
                    assert(my_tree.size() == 0);

                    BalanceTree<int> my_tree_copy = my_tree_move;
                    assert(my_tree_move.is_same(my_tree_copy) == true);

                    my_tree = std::move(my_tree_copy);
                    assert(my_tree.is_same(my_tree_move) == true);
                    assert(my_tree_copy.get_root() == nullptr);
                    assert(my_tree_copy.size() == 0);
                }

                {
                    BalanceTree<int> my_tree_move(rang_index);
                    my_tree_move = std::move(my_tree);
                    assert(my_tree.get_root() == nullptr);
                    assert(my_tree.size() == 0);

                    BalanceTree<int> my_tree_copy(rang_index);
                    my_tree_copy = my_tree_move;
                    assert(my_tree_move.is_same(my_tree_copy) == true);

                    my_tree = std::move(my_tree_copy);
                    assert(my_tree.is_same(my_tree_move) == true);

                    assert(my_tree_copy.get_root() == nullptr);
                    assert(my_tree_copy.size() == 0);
                }

                for (auto& i : array)
                {
                    if (my_tree.remove(i))
                        ele_size --;

                    if (!my_tree.is_b_tree())
                    {
                        stream << "remove: "<< i << " failed. " << std::endl;
                        my_tree.print_tree();
                        assert(false);
                    }
                }
                assert(my_tree.get_root() == nullptr);
                assert(my_tree.size() == 0);
                assert(my_tree.memory_alloc_balance() == true);
                stream << "range_index : "<< rang_index << std::endl;
            }
        }
        stream << "last rang_index: "<< rang_index << std::endl;
    }
#endif

#if 1
    {
        using namespace tools::b_tree2;
        stream << "tools::b_tree2" << std::endl;
        size_t ele_size = 0;
        int rang_index = 10;
        while(--rang_index > 1)
        {
            BalanceTree<int> my_tree(rang_index);
            int num = 5;
            while (num -- > 0)
            {
                Vector<int> array;
                Rand<int> rand(1, 10000);
                for(int i = 0; i < 1000; i++)
                {
                    array.push_back(rand());
                }

                // for (auto i : array)
                // {
                //     stream << i << ", ";
                // }
                // stream << std::endl;
                
                for (auto& i : array)
                {
                    if (my_tree.insert(i))
                        ele_size ++;

                    // my_tree.print_tree();
                    // stream << "insert: "<< i << std::endl;

                    if (!my_tree.is_b_tree())
                    {
                        stream << "insert: "<< i << " failed. " << std::endl;
                        my_tree.print_tree();
                        assert(false);
                    }

                    assert(my_tree.InOrder() == my_tree.size());
                    assert(my_tree.PrevInOrder() == my_tree.size());
                }

                for (auto& i : array)
                {
                    assert(my_tree.search(i) == true);
                }

                {
                    BalanceTree<int> my_tree_move = std::move(my_tree);
                    assert(my_tree.get_root() == nullptr);
                    assert(my_tree.size() == 0);

                    BalanceTree<int> my_tree_copy = my_tree_move;
                    assert(my_tree_move.is_same(my_tree_copy) == true);

                    my_tree = std::move(my_tree_copy);
                    assert(my_tree.is_same(my_tree_move) == true);
                    assert(my_tree_copy.get_root() == nullptr);
                    assert(my_tree_copy.size() == 0);
                }

                {
                    BalanceTree<int> my_tree_move(rang_index);
                    my_tree_move = std::move(my_tree);
                    assert(my_tree.get_root() == nullptr);
                    assert(my_tree.size() == 0);

                    BalanceTree<int> my_tree_copy(rang_index);
                    my_tree_copy = my_tree_move;
                    assert(my_tree_move.is_same(my_tree_copy) == true);

                    my_tree = std::move(my_tree_copy);
                    assert(my_tree.is_same(my_tree_move) == true);

                    assert(my_tree_copy.get_root() == nullptr);
                    assert(my_tree_copy.size() == 0);
                }
                for (auto& i : array)
                {
                    if (my_tree.remove(i))
                        ele_size --;

                    if (!my_tree.is_b_tree())
                    {
                        stream << "remove: "<< i << " failed. " << std::endl;
                        my_tree.print_tree();
                        assert(false);
                    }
                    assert(my_tree.InOrder() == my_tree.size());
                    assert(my_tree.PrevInOrder() == my_tree.size());
                }
                assert(my_tree.get_root() == nullptr);
                assert(my_tree.size() == 0);
                assert(my_tree.memory_alloc_balance() == true);
                stream << "range_index : "<< rang_index << std::endl;
            }
        }
        stream << "last rang_index: "<< rang_index << std::endl;
    }
#endif

#if 1
    {
        using namespace tools::bp_tree;
        stream << "tools::bp_tree" << std::endl;
        size_t ele_size = 0;
        int rang_index = 10;
        while(--rang_index > 1)
        {
            BalancePlusTree<int> my_tree(rang_index);
            int num = 5;
            while (num -- > 0)
            {
                Vector<int> array;
                Rand<int> rand(1, 10000);
                for(int i = 0; i < 1000; i++)
                {
                    array.push_back(rand());
                }

                // for (auto i : array)
                // {
                //     stream << i << ", ";
                // }
                // stream << std::endl;
                
                for (auto& i : array)
                {
                    if (my_tree.insert(i))
                        ele_size ++;

                    // my_tree.print_tree();
                    // stream << "insert: "<< i << std::endl;

                    if (!my_tree.is_b_plus_tree())
                    {
                        stream << "insert: "<< i << " failed. " << std::endl;
                        my_tree.print_tree();
                        assert(false);
                    }
                }

                for (auto& i : array)
                {
                    assert(my_tree.search(i) == true);
                }

                {
                    BalancePlusTree<int> my_tree_move = std::move(my_tree);
                    assert(my_tree.get_root() == nullptr);
                    assert(my_tree.size() == 0);

                    BalancePlusTree<int> my_tree_copy = my_tree_move;
                    assert(my_tree_move.is_same(my_tree_copy) == true);

                    my_tree = std::move(my_tree_copy);
                    assert(my_tree.is_same(my_tree_move) == true);
                    assert(my_tree_copy.get_root() == nullptr);
                    assert(my_tree_copy.size() == 0);
                }

                {
                    BalancePlusTree<int> my_tree_move(rang_index);
                    my_tree_move = std::move(my_tree);
                    assert(my_tree.get_root() == nullptr);
                    assert(my_tree.size() == 0);

                    BalancePlusTree<int> my_tree_copy(rang_index);
                    my_tree_copy = my_tree_move;
                    assert(my_tree_move.is_same(my_tree_copy) == true);

                    my_tree = std::move(my_tree_copy);
                    assert(my_tree.is_same(my_tree_move) == true);

                    assert(my_tree_copy.get_root() == nullptr);
                    assert(my_tree_copy.size() == 0);
                }

                for (auto& i : array)
                {
                    if (my_tree.remove(i))
                        ele_size --;

                    if (!my_tree.is_b_plus_tree())
                    {
                        stream << "remove: "<< i << " failed. " << std::endl;
                        my_tree.print_tree();
                        assert(false);
                    }
                }
                assert(my_tree.get_root() == nullptr);
                assert(my_tree.memory_alloc_balance() == true);
                assert(my_tree.size() == 0);
                stream << "range_index : "<< rang_index << std::endl;
            }
        }
        stream << "last rang_index: "<< rang_index << std::endl;
    }
#endif

#if 1
    {
        using namespace tools::bp_tree2;
        stream << "tools::bp_tree2" << std::endl;
        size_t ele_size = 0;
        int rang_index = 10;
        while(--rang_index > 1)
        {
            BalancePlusTree<int> my_tree(rang_index);
            int num = 5;
            while (num -- > 0)
            {
                Vector<int> array;
                Rand<int> rand(1, 10000);
                for(int i = 0; i < 1000; i++)
                {
                    array.push_back(rand());
                }

                // for (auto i : array)
                // {
                //     stream << i << ", ";
                // }
                // stream << std::endl;
                
                for (auto& i : array)
                {
                    if (my_tree.insert(i))
                        ele_size ++;

                    // my_tree.print_tree();
                    // stream << "insert: "<< i << std::endl;

                    if (!my_tree.is_b_plus_tree())
                    {
                        stream << "insert: "<< i << " failed. " << std::endl;
                        my_tree.print_tree();
                        assert(false);
                    }

                    assert(my_tree.InOrder() == my_tree.size());
                    assert(my_tree.PrevInOrder() == my_tree.size());
                }

                for (auto& i : array)
                {
                    assert(my_tree.search(i) == true);
                }

                {
                    BalancePlusTree<int> my_tree_move = std::move(my_tree);
                    assert(my_tree.get_root() == nullptr);
                    assert(my_tree.size() == 0);

                    BalancePlusTree<int> my_tree_copy = my_tree_move;
                    assert(my_tree_move.is_same(my_tree_copy) == true);

                    my_tree = std::move(my_tree_copy);
                    assert(my_tree.is_same(my_tree_move) == true);
                    assert(my_tree_copy.get_root() == nullptr);
                    assert(my_tree_copy.size() == 0);
                }

                {
                    BalancePlusTree<int> my_tree_move(rang_index);
                    my_tree_move = std::move(my_tree);
                    assert(my_tree.get_root() == nullptr);
                    assert(my_tree.size() == 0);

                    BalancePlusTree<int> my_tree_copy(rang_index);
                    my_tree_copy = my_tree_move;
                    assert(my_tree_move.is_same(my_tree_copy) == true);

                    my_tree = std::move(my_tree_copy);
                    assert(my_tree.is_same(my_tree_move) == true);

                    assert(my_tree_copy.get_root() == nullptr);
                    assert(my_tree_copy.size() == 0);
                }

                for (auto& i : array)
                {
                    if (my_tree.remove(i))
                        ele_size --;

                    if (!my_tree.is_b_plus_tree())
                    {
                        stream << "remove: "<< i << " failed. " << std::endl;
                        my_tree.print_tree();
                        assert(false);
                    }
                    assert(my_tree.InOrder() == my_tree.size());
                    assert(my_tree.PrevInOrder() == my_tree.size());
                }
                assert(my_tree.get_root() == nullptr);
                assert(my_tree.memory_alloc_balance() == true);
                assert(my_tree.size() == 0);
                stream << "range_index : "<< rang_index << std::endl;
            }
        }
        stream << "last rang_index: "<< rang_index << std::endl;
    }
#endif

    return 0;
}
