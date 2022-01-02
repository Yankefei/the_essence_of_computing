#include "stream.h"

#include "binary_thread_tree.h"

using namespace tools;

int main()
{
    #if 1
    {
        BtTree<> test;

        auto p = test.new_node('A');
        test.free_node(p);

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

    }

    return 0;
}