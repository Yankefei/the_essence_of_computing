#ifndef _TOOLS_BINARY_AVL_TREE_UTIL_H_
#define _TOOLS_BINARY_AVL_TREE_UTIL_H_

#include <memory>
#include <cassert>

#include "vector.hpp"
#include "queue.h"

#include "binary_tree_util.h"

namespace tools
{

template<typename Node>
struct _DrawNode
{
    _DrawNode(Node* _ptr, uint32_t _level, uint32_t _index)
        : ptr(_ptr), level_num(_level), index_of_level(_index)
        {}
    Node* ptr;
    uint32_t level_num;
    uint32_t index_of_level;
};

void print_empty(uint32_t num)
{
    while(num-- > 0)
    {
        stream << " ";
    }
}

template<typename Node>
void draw_tree(Node* ptr)
{
    using DrawNode = _DrawNode<Node>;
    if (ptr == nullptr) return;

    uint32_t level_num = 1;

    Vector<DrawNode> tree_draw_info;
    Queue<DrawNode> tmp_draw_queue;
    Queue<DrawNode> tmp_draw_queue2;
    tmp_draw_queue.push(DrawNode(ptr, 1, 1));

    // 交替赋值
    while(!tmp_draw_queue.empty() || tmp_draw_queue2.empty())
    {
        uint32_t index_of_level = 1;
        bool is_finish = true;
        while (!tmp_draw_queue.empty())
        {
            DrawNode node = tmp_draw_queue.front();
            tmp_draw_queue.pop_front();

            if (node.ptr && node.ptr->left_tree_)
            {
                tmp_draw_queue2.push(
                    DrawNode(node.ptr->left_tree_, node.level_num + 1, index_of_level ++));
                is_finish = false;
            }
            else
            {
                tmp_draw_queue2.push(
                    DrawNode(nullptr, node.level_num + 1, index_of_level ++));
            }

            if (node.ptr && node.ptr->right_tree_)
            {
                tmp_draw_queue2.push(
                    DrawNode(node.ptr->right_tree_, node.level_num + 1, index_of_level ++));
                is_finish = false;
            }
            else
            {
                tmp_draw_queue2.push(
                    DrawNode(nullptr, node.level_num + 1, index_of_level ++));
            }
            // 多打印一行
            tree_draw_info.emplace_back(node);
        }

        level_num ++;
        if (is_finish)
            break;
        
        index_of_level = 1;
        is_finish = true;
        while(!tmp_draw_queue2.empty())
        {
            DrawNode node = tmp_draw_queue2.front();
            tmp_draw_queue2.pop_front();

            if (node.ptr && node.ptr->left_tree_)
            {
                tmp_draw_queue.push(
                    DrawNode(node.ptr->left_tree_, node.level_num + 1, index_of_level ++));
                is_finish = false;
            }
            else
            {
                tmp_draw_queue.push(
                    DrawNode(nullptr, node.level_num + 1, index_of_level ++));
            }

            if (node.ptr && node.ptr->right_tree_)
            {
                tmp_draw_queue.push(
                    DrawNode(node.ptr->right_tree_, node.level_num + 1, index_of_level ++));
                is_finish = false;
            }
            else
            {
                tmp_draw_queue.push(
                    DrawNode(nullptr, node.level_num + 1, index_of_level ++));
            }

            // 多打印一行
            tree_draw_info.emplace_back(node);
        }

        level_num ++;
        if (is_finish)
            break;
    }

    uint32_t max_num = power(2, level_num - 1);
    uint32_t max_line = max_num * 2;

    uint32_t line_index = 1;
    uint32_t interval_num = max_line / (power(2, line_index - 1)); // 每一行的间隔数
    for (auto & node : tree_draw_info)
    {
        if (line_index != node.level_num)
        {
            stream << std::endl << std::endl;
            line_index  = node.level_num; // 按顺序递增
            interval_num = max_line / (power(2, line_index - 1));
        }

        print_empty(interval_num / 2);
        if (node.ptr)
            stream << node.ptr->data_;
        else
            stream << " ";
        print_empty(interval_num / 2 - 1);
    }
    stream << std::endl << std::endl;
}

}


#endif
