#include "stream.h"
#include "stack.h"

#include "queue.h"

#include "binary_tree_util.h"
#include "binary_tree2.h"

#include <cstring>

using namespace tools;

int main()
{
#if 1
    {
        stream << "my_tree" << std::endl;
        BinaryTree2<> my_tree('#');

        //char str[] = "ABCD###EF##GH###I#JK#L###";
        //char str[] = "ABCD##E##F##G##";
        //char str[] = "ABD##E##CF###";
        //char str[] = "ABD##E##CF##G##";
        char str[] = "ABCDEF#######";
        //char str[] = "ABC###D##";
        my_tree.create_tree(str, OrderType::PreOrder);
        // my_tree.print_tree(OrderType::PreOrder);
        // my_tree.print_tree(OrderType::InOrder);
        // my_tree.print_tree(OrderType::LastOrder);

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

        {
            BinaryTree2<> my_tree_copy = my_tree;
            stream << "my_tree_copy deep: "<< my_tree_copy.max_deep_len() << std::endl;
            stream << "my_tree_copy size: "<< my_tree_copy.size() << std::endl;

            my_tree_copy = my_tree;
            stream << "my_tree_copy deep: "<< my_tree_copy.max_deep_len() << std::endl;
            stream << "my_tree_copy size: "<< my_tree_copy.size() << std::endl;
        

            stream << Boolalpha << BinaryTree2<>::is_equal(my_tree_copy, my_tree) << std::endl;


            BinaryTree2<> my_tree2('#');

            char str[] = "ABCD###EF##GH###I#JK#L###";
            my_tree2.create_tree(str, OrderType::PreOrder);
            stream << BinaryTree2<>::is_equal(my_tree_copy, my_tree2) << std::endl;

            stream << NoBoolalpha;

            BTNInIterator<> ni(my_tree2);
            Print(ni);
        }
    }
#endif
}