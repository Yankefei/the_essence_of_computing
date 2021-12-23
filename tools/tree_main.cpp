#include "stream.h"
#include "stack.h"

#include "queue.h"

#include "binary_tree.h"
#include "tree_util.h"

using namespace tools;

int main()
{
#if 1
    {
        stream << "my_tree" << std::endl;
        BinaryTree<> my_tree('#');

        //char str[] = "ABCD###EF##GH###I#JK#L###";
        char str[] = "ABCD##E##F##G##";
        //char str[] = "ABD##E##CF###";
        //char str[] = "ABD##E##CF##G##";
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
    
        BtNode<char>* f_ptr = nullptr;
        assert(my_tree.find_val('F', f_ptr));
        BtNode<char>* d_ptr = nullptr;
        assert(my_tree.find_val('D', d_ptr));
        BtNode<char>* com_parent =
            FindComParent(my_tree.get_root(), f_ptr, d_ptr);
        assert(com_parent != nullptr);
        stream << "F D comParent: "<< com_parent->data_ << std::endl;
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

    return 0;
}