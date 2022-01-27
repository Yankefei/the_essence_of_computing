#ifndef _TOOLS_BALANCE_TREE_PRINT_UTIL_H_
#define _TOOLS_BALANCE_TREE_PRINT_UTIL_H_

#include <memory>
#include <cassert>

#include "stream.h"

#include "vector.hpp"
#include "queue.h"

namespace tools
{

void print_table_empty(int tab_size)
{
    for (int i = 0; i < tab_size; i++)
        stream << " ";
}

/*B树的前序遍历*/
template<typename Node>
void pre_order_tree(Node* ptr, int32_t hight, int32_t total_hight, int32_t m)
{
    if (ptr == nullptr) return;

    assert(ptr->size_ <= m);
    const int tab_size = 4;  //缩进的大小
    int i = 0;
    print_table_empty((total_hight - hight) * tab_size); // 打印缩进
    for (; i < ptr->size_; i++)
        stream << ptr->array_[i]->data_ <<",";
    for (; i < m; i ++)
        stream << " _,";
    stream << std::endl;

    for (i = 0; i < ptr->size_; i++)
    {
        pre_order_tree(ptr->array_[i]->next_, hight - 1, total_hight, m);
    }
}

template<typename Node>
void draw_tree(Node* ptr, int32_t hight, int32_t m)
{
    pre_order_tree(ptr, hight, hight, m);
    stream << std::endl;
}

}

#endif