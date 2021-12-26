#include "stream.h"
#include "stack.h"

#include "queue.h"

#include "binary_tree.h"
#include "binary_tree_util.h"

#include <cstring>

using namespace tools;

int main()
{
#if 1
    {
        stream << "my_tree" << std::endl;
        BinaryTree<> my_tree('#');

        //char str[] = "ABCD###EF##GH###I#JK#L###";
        //char str[] = "ABCD##E##F##G##";
        //char str[] = "ABD##E##CF###";
        //char str[] = "ABD##E##CF##G##";
        char str[] = "ABCDEF#######";
        //char str[] = "ABC###D##";
        my_tree.create_tree(str, OrderType::PreOrder);
        my_tree.print_tree(OrderType::PreOrder);
        my_tree.print_tree(OrderType::InOrder);
        my_tree.print_tree(OrderType::LastOrder);

        stream << "deep: "<< my_tree.max_deep_len() << std::endl;
        stream << "size: "<< my_tree.size() << std::endl;

        // bool full = Is_Full_Binary_Tree1(my_tree.size(), my_tree.max_deep_len());
        // stream <<"full: "<< (full ? "true" : "false") << std::endl;
    
        bool full = Is_Full_Binary_Tree2(my_tree.get_root());
        stream <<"full: "<< (full ? "true" : "false") << std::endl;
    
        bool comp = Is_Comp_Binary_Tree(my_tree.get_root());
        stream << "comp: " << (comp ? "true" : "false") << std::endl;
    
        // BtNode<char>* f_ptr = nullptr;
        // assert(my_tree.find_val('F', f_ptr));
        // BtNode<char>* d_ptr = nullptr;
        // assert(my_tree.find_val('D', d_ptr));
        // BtNode<char>* com_parent =
        //     FindComParent(my_tree.get_root(), f_ptr, d_ptr);
        // assert(com_parent != nullptr);
        // stream << "F D comParent: "<< com_parent->data_ << std::endl;
    
        BtNode<char>* begin_path = nullptr;
        BtNode<char>* end_path = nullptr;
        int len = MaxPath3(my_tree.get_root(), begin_path, end_path);
        //int len = MaxPath2(my_tree.get_root());
        //stream << "max_len: " << len << std::endl;

        assert(begin_path != nullptr && end_path != nullptr);
        stream << "max_len: " << len <<" "<< begin_path->data_ << " to " << end_path->data_ << std::endl;
    
        {
            size_t node_num = my_tree.size();
            BtNode<char>* array = new BtNode<char>[node_num];
            ::memset(array, 0, sizeof(BtNode<char>) * node_num);

            BinaryTreeToList(my_tree.get_root(), array, node_num);

            BtNode<char>* _begin_path = nullptr;
            BtNode<char>* _end_path = nullptr;
            int _len = MaxPath3(array, _begin_path, _end_path);

            assert(_begin_path != nullptr && _end_path != nullptr);
            stream << "array_tree max_len: " << _len <<" "<< _begin_path->data_
                   << " to " << _end_path->data_ << std::endl;

            delete[] array;
        }
    }
#endif

#if 0
    {
        stream << "my_tree2" << std::endl;
        BinaryTree<> my_tree2('#');
        char pre_str[] = "ABCDEFGHIJKL";
        char in_str[] =  "DCBFEHGAIKLJ";
        my_tree2.create_tree_pre_in(pre_str, in_str, 12);
        // my_tree2.print_tree(OrderType::PreOrder);
        // // // D C B F E H G A I K L J
        my_tree2.print_tree(OrderType::InOrder);

        // my_tree2.print_tree(OrderType::LastOrder);

        //my_tree2.print_tree(OrderType::LevelOrder);

        //tools::level_order_retrace<char>(my_tree2.get_root());

        //my_tree2.StkNicePastOrder(my_tree2.get_root());
        //my_tree2.StkNiceInOrder(my_tree2.get_root());
    }

    {
        stream << "my_tree3" << std::endl;
        BinaryTree<> my_tree3;
        char in_str[]   = "DCBFEHGAIKLJ";
        char last_str[] = "DCFHGEBLKJIA";
        my_tree3.create_tree_in_last(in_str, last_str, 12);
        my_tree3.print_tree(OrderType::PreOrder);

        stream << "size: " << my_tree3.size() << std::endl;
        stream << "max_deep_len: "<< my_tree3.max_deep_len() << std::endl;
    
        // my_tree3.clean();
        // my_tree3.print_tree(OrderType::PreOrder);
        // stream << "size: " << my_tree3.size() << std::endl;
        // stream << "max_deep_len: "<< my_tree3.max_deep_len() << std::endl;
    
        stream << "has C? " << (my_tree3.find_val('C') ? "yes" : "no") << std::endl;
        stream << "has U? " << (my_tree3.find_val('U') ? "yes" : "no") << std::endl;
    
        char parent1 = 0, parent2 = 0;
        bool find = my_tree3.find_parent('C', parent1);
        if (find)
            stream << "C parent: "<< parent1 << std::endl;  // yes
        find = my_tree3.find_parent('A', parent2);
        if (find)
            stream << "A parent: "<< parent2 << std::endl;  // no

        
    }

#endif

    {
#if 0
        BtNode<char>* begin_path = nullptr;
        BtNode<char>* end_path = nullptr;

        // P. 241 Graph 3-12
        BtNode<> test1[9] = { 0 };
        test1[7].data_ = 'A';
        test1[8].data_ = 'B';
        Link(test1, 0, 1, 2);
        Link(test1, 1, 3, 4);
        Link(test1, 2, 5, 6);
        Link(test1, 3, 7, -1);
        Link(test1, 5, -1, 8);
        int len = MaxPath3(&test1[0], begin_path, end_path);
        assert(begin_path != nullptr && end_path != nullptr);
        stream << "max_len: " << len <<" "<< begin_path->data_ << " to " << end_path->data_ << std::endl;
    
        // P. 242 Graph 3-13 left
        begin_path = nullptr;
        end_path = nullptr;
        BtNode<> test2[4] = { 0 };
        test2[2].data_ = 'A';
        test2[3].data_ = 'B';
        Link(test2, 0, 1, 2);
        Link(test2, 1, 3, -1);
        len = MaxPath3(&test2[0], begin_path, end_path);
        assert(begin_path != nullptr && end_path != nullptr);
        stream << "max_len: " << len <<" "<< begin_path->data_ << " to " << end_path->data_ << std::endl;
    
        // P. 242 Graph 3-13 right
        begin_path = nullptr;
        end_path = nullptr;
        BtNode<> test3[9] = { 0 };
        for (int i = 0; i < 9; i++)
        {
            test3[i].data_ = 'A' + i;
        }
        // error
        Link(test3, 0, -1, 1);
        Link(test3, 1, 2, 3);
        Link(test3, 2, 4, -1);
        Link(test3, 3, 5, 6);
        Link(test3, 4, 7, -1);
        Link(test3, 5, -1, 8);
        len = MaxPath3(&test3[0], begin_path, end_path);
        assert(begin_path != nullptr && end_path != nullptr);
        stream << "max_len: " << len <<" "<< begin_path->data_ << " to " << end_path->data_ << std::endl;
    
        // P. 242 Graph 3-14
        // Same as Graph 3-2, not test
    
        // P. 243 Graph 3-15
        begin_path = nullptr;
        end_path = nullptr;
        BtNode<> test4[9] = { 0 };
        test4[2].data_ = 'A';
        test4[7].data_ = 'B';
        test4[8].data_ = 'C';
        Link(test4, 0, 1, 2);
        Link(test4, 1, 3, 4);
        Link(test4, 3, 5, 6);
        Link(test4, 5, 7, -1);
        Link(test4, 6, -1, 8);
        len = MaxPath3(&test4[0], begin_path, end_path);
        assert(begin_path != nullptr && end_path != nullptr);
        stream << "max_len: " << len <<" "<< begin_path->data_ << " to " << end_path->data_ << std::endl;
#endif
    }

    {
        BtNode<> test1[7] = {0};
        for (int i = 0; i < 7; i ++)
            test1[i].data_ = 'A' + i;
        
        Link(test1, 0, 1, 2);
        Link(test1, 1, 3, 4);
        Link(test1, 2, 5, 6);

        PrintCompBTree(test1, 2);

        BtNode<> test2[12] = {0};
        for (int i = 0; i < 12; i ++)
            test2[i].data_ = 'A' + i;
        
        Link(test2, 0, 1, 2);
        Link(test2, 1, 3, 4);
        Link(test2, 2, -1, 5);
        Link(test2, 3, 6, -1);
        Link(test2, 4, 7, 8);
        Link(test2, 5, 9, -1);
        Link(test2, 8, 10, -1);
        Link(test2, 9, -1, 11);

        stream << "level: "<< GetBinaryTreeLevel(test2, 4) << std::endl; 
    
        bool test1_flag = IsBalance_BinaryTree(test1);
        stream << "test1: " << (test1_flag ? "true": "false") << std::endl;

        bool test2_flag = IsBalance_BinaryTree(test2);
        stream << "test2: " << (test2_flag ? "true": "false") << std::endl;


        BtNode<> test3[8] = {0};
        for (int i = 0; i < 7; i ++)
            test3[i].data_ = 'A' + i;
        
        Link(test3, 0, 1, 2);
        Link(test3, 1, 3, 4);
        Link(test3, 2, 5, 6);
        Link(test3, 3, 7, -1);
        bool test3_flag = IsBalance_BinaryTree(test3);
        stream << "test3: " << (test3_flag ? "true": "false") << std::endl;


        BtNode<> test4[7] = {0};
        for (int i = 0; i < 6; i ++)
            test4[i].data_ = 'A' + i;
        
        Link(test4, 0, 1, 2);
        Link(test4, 1, 3, 4);
        Link(test4, 2, 5, -1);
        Link(test4, 3, 6, -1);
        bool test4_flag = IsBalance_BinaryTree(test4);
        stream << "test4: " << (test4_flag ? "true": "false") << std::endl;


        BtNode<> test5[6] = {0};
        for (int i = 0; i < 5; i ++)
            test5[i].data_ = 'A' + i;
        
        Link(test5, 0, 1, 2);
        Link(test5, 1, 3, 4);
        Link(test5, 3, 5, -1);
        bool test5_flag = IsBalance_BinaryTree(test5);
        stream << "test5: " << (test5_flag ? "true": "false") << std::endl;
    }

    return 0;
}