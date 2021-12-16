#include "stream.h"
#include "stack.h"

#include "queue.h"

#include "binary_tree.h"

using namespace tools;

int main()
{
    BinaryTree<> my_tree('#');

    char str[] = "ABC##DE##F##G#H##";
    my_tree.CreateTree(str, CreateType::PreOrder);
    my_tree.PrintTree(CreateType::PreOrder);

    return 0;
}