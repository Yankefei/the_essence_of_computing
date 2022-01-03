#include "stream.h"

#include "binary_thread_tree.h"

using namespace tools;

int main()
{
    #if 0
    {
        stream << sizeof(BtTree<>) << std::endl;

        stream << sizeof(BtTree_Base<char, std::allocator<char>>) << std::endl;
        stream << sizeof(BtTree_Base<char, std::allocator<char>>::Base_Impl) << std::endl;
        stream << sizeof(BtTree_Base<char, std::allocator<char>>::_m_impl) << std::endl;

        stream << sizeof(BtTree_Base<char, std::allocator<char>>::_TNode_alloc_type) << std::endl;
        stream << sizeof(char) << std::endl;
        stream << sizeof(BtTree_Base<char, std::allocator<char>>::Node) << std::endl;
    }
    #endif

    {
        BtTree<> test('#');

        char str[] = "ABCD###EF##GH###I#JK#L###";

        test.create_tree_by_pre(str);
        test.InOrder();
        test.ResInOrder();

        BtTInIterator<> tini(test);
        Print(tini);
        ResPrint(tini);
    }

    return 0;
}