#include "stream.h"

#include "binary_rb_tree.h"
#include "vector.hpp"

using namespace tools;
using namespace tools::rb_tree_1;


int main()
{
    stream << Boolalpha;
    {
        RbTree<int> my_tree;

        stream << "sizeof RbTree<int> is "<< sizeof(RbTree<int>) << std::endl;

        Vector<int> array{10, 85, 15, 70, 20, 60, 30, 50, 65, 80, 90, 40, 5, 55, 45};
        for (auto e : array)
        {
            my_tree.insert(e);
            stream << "after insert "<< e <<"is_rb_tree ? "<< my_tree.is_rb_tree() << std::endl;
            my_tree.print_tree();
        }

    }

    return 0;
}