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

template<typename Node>
void print_tree_node(Node* ptr, int32_t m)
{
    if (ptr == nullptr) return;
    int i = 0;
    for (; i < ptr->size_; i++)
        stream << ptr->array_[i]->data_ <<",";
    for (; i < m; i ++)
        stream << " _,";
}

template<typename Node>
void level_print_tree_node(Node* ptr, int32_t hight, int32_t m)
{
    if (ptr == nullptr) return;
    //stream << "**"<< hight << "** ";
    for (int i = 0; i < ptr->size_; i++)
    {
        Node* n_ptr = ptr->array_[i]->next_;
        print_tree_node(n_ptr, m);
        stream << "  ";
    }
}

/*B树的前序遍历*/
template<typename Node>
void pre_order_tree1(Node* ptr, int32_t hight, int32_t total_hight, int32_t m)
{
    if (ptr == nullptr) return;

    assert(ptr->size_ <= m);
    const int empty_size = 4;  //缩进的大小
    int i = 0;
    print_table_empty((total_hight - hight) * empty_size); // 打印缩进
    print_tree_node(ptr, m);
    stream << std::endl;

    // 单独处理最后一行，横向打印
    if (hight == 1)
        print_table_empty((total_hight - hight + 1) * empty_size);

    for (i = 0; i < ptr->size_; i++)
    {
        if (hight > 1)
            pre_order_tree1(ptr->array_[i]->next_, hight - 1, total_hight, m);
        else
        {
            Node* n_ptr = ptr->array_[i]->next_;
            print_tree_node(n_ptr, m);
            stream << "  ";
        }
    }
    if (hight == 1) stream << std::endl;
}

/*B树的前序遍历*/
// 使用复合的方式将每一行都能平铺展示，用来调试问题
template<typename Node>
void pre_order_tree2(Node* ptr, int32_t hight, int32_t total_hight, int32_t m)
{
    if (ptr == nullptr) return;

    assert(ptr->size_ <= m);
    const int empty_size = 4;  //缩进的大小
    int table_size = total_hight - hight;

    if (hight > 0)
    {
        print_table_empty((table_size + 1) * empty_size); // 打印缩进
        print_tree_node(ptr, m);
        stream << std::endl;

        if (hight != 1)
            print_table_empty((table_size + 2) * empty_size); // 打印缩进
        else
            print_table_empty((table_size + 3) * empty_size);
        level_print_tree_node(ptr, hight - 1, m);
        stream << std::endl;
        stream << std::endl;
    }

    for (int j = 0; j < ptr->size_; j++)
    {
        pre_order_tree2(ptr->array_[j]->next_, hight - 1, total_hight, m);
    }
}

template<typename Node>
void draw_tree(Node* ptr, int32_t hight, int32_t m)
{
    // pre_order_tree1(ptr, hight, hight, m);
    if (hight > 0)
        pre_order_tree2(ptr, hight, hight, m);
    else
    {
        static  int empty_size = 4;  //缩进的大小
        print_table_empty(empty_size); // 打印缩进
        print_tree_node(ptr, m);
    }

    stream << std::endl;
}

// 紧凑版
template<typename Node>
void draw_tree1(Node* ptr, int32_t hight, int32_t m)
{
    pre_order_tree1(ptr, hight, hight, m);
    stream << std::endl;
}

// template<typename Node>
// void draw_tree2(Node* ptr, int32_t hight, int32_t m)
// {
//     level_order_tree(ptr, hight, m);
//     stream << std::endl;
// }

}

#endif