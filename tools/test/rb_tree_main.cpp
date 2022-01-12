#include "stream.h"

#include "binary_rb_tree.h"

using namespace tools;
using namespace tools::rb_tree_1;


int main()
{
    RbTree<int> my_tree;

    stream << "sizeof RbTree<int> is "<< sizeof(RbTree<int>) << std::endl;

    my_tree.print_tree();

    return 0;
}