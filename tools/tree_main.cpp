#include "stream.h"
#include "stack.h"

#include "queue.h"

#include "binary_tree.h"

using namespace tools;

int main()
{
    {
        stream << "my_tree" << std::endl;
        BinaryTree<> my_tree('#');

        char str[] = "ABC##DE##F##G#H##";
        my_tree.create_tree(str, CreateType::PreOrder);
        my_tree.print_tree(CreateType::PreOrder);
        my_tree.print_tree(CreateType::InOrder);
        my_tree.print_tree(CreateType::LastOrder);
    }

    {
        stream << "my_tree2" << std::endl;
        BinaryTree<> my_tree2('#');
        char pre_str[] = "ABCDEFGH";
        char in_str[] =  "CBEDFAGH";
        my_tree2.create_tree_pre_in(pre_str, in_str, 8);
        my_tree2.print_tree(CreateType::PreOrder);
    }

    {
        stream << "my_tree3" << std::endl;
        BinaryTree<> my_tree3('#');
        char in_str[]   = "CBEDFAGH";
        char last_str[] = "CEFDBHGA";
        my_tree3.create_tree_in_last(in_str, last_str, 8);
        my_tree3.print_tree(CreateType::PreOrder);

        stream << "size: " << my_tree3.size() << std::endl;
        stream << "max_deep_len: "<< my_tree3.max_deep_len() << std::endl;
    
        // my_tree3.clean();
        // my_tree3.print_tree(CreateType::PreOrder);
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

    return 0;
}