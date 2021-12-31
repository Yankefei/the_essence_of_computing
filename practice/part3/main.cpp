#include "stream.h"

using namespace tools;

extern void PrintTreeLeafNode(const char* ptr, int len);

int main()
{
    char array[] = "abcd";
    PrintTreeLeafNode(array, 4);
    return 0;
}