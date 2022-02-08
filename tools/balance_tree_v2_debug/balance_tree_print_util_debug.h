#ifndef _TOOLS_BALANCE_TREE_V2_PRINT_UTIL_DEBUG_H_
#define _TOOLS_BALANCE_TREE_V2_PRINT_UTIL_DEBUG_H_

#include <memory>
#include <cassert>

#include "stream.h"

#include "vector.hpp"
#include "queue.h"

namespace tools
{

template<typename Node>
class DrawBalanceTreeV2
{
public:
    DrawBalanceTreeV2(Node* ptr, int32_t hight, int32_t m)
        : root_(ptr), hight_(hight), m_(m)
    {}

    ~DrawBalanceTreeV2() {}

    void draw()
    {
        in_order_tree(root_, hight_);
        stream << std::endl;
    }

private:
    void print_table_empty(int tab_size)
    {
        for (int i = 0; i < tab_size; i++)
        {
            // 打印连线或者空格
            if (no_leaf_node_status_[i] == 1)
                stream << "|";
            else
                stream << " ";
        }
    }

    void print_tree_node(Node* ptr)
    {
        if (ptr == nullptr) return;
        int i = 0;
        for (; i < ptr->size_; i++)
            stream << ptr->array_[i].data_ <<",";
        for (; i < m_; i ++)
            stream << " _,";
    }

    /*B树的中序遍历*/
    void in_order_tree(Node* ptr, int32_t hight)
    {
        if (ptr == nullptr) return;

        assert(ptr->size_ <= m_ -1);
        static int empty_size = 8; // 缩进的格式大小
        int table_size = (hight_ - hight + 1) * empty_size;

        if (table_size + 1 > no_leaf_node_status_.size())
        {
            int32_t diff = table_size + 1 - no_leaf_node_status_.size();
            for (int32_t i = 0; i < diff; i++)
                no_leaf_node_status_.push_back(0);
        }

        if (hight == 0)
        {
            print_table_empty(table_size); // 打印缩进
            print_tree_node(ptr);
        }
        else
        {
            int i = 0;
            for (; i < ptr->size_; i++)
            {
                in_order_tree(ptr->array_[i].next_, hight - 1);

                print_table_empty(table_size); // 打印缩进
                stream << ptr->array_[i].data_ << std::endl;
                no_leaf_node_status_[table_size] = 1;
            }
            no_leaf_node_status_[table_size] = 0;
            in_order_tree(ptr->array_[i].next_, hight - 1);
        }
        stream << std::endl;
    }

private:
    Node*    root_;
    int32_t  hight_;
    int32_t  m_;

    Vector<int32_t> no_leaf_node_status_; // 记录非叶子节点的状态值, 用于标识是否画出连线
};

/*
效果如下：3阶v2版的B树，有连线标识在非叶子节点中该数据是属于一个Node。

                        0, _, _,
                1
                        2, _, _,

        3
        |               4, _, _,
        |       5
        |               6, _, _,

        7
                        8, _, _,
                9
                |       10, _, _,
                11
                        12,13, _,
*/

template<typename Node>
void draw_tree(Node* ptr, int32_t hight, int32_t m)
{
    DrawBalanceTreeV2<Node> braw_tree(ptr, hight, m);
    braw_tree.draw();
}

}

#endif