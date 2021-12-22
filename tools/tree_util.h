#ifndef _TOOLS_TREE_UTIL_H_
#define _TOOLS_TREE_UTIL_H_

#include <memory>
#include <cassert>

#include <stack>
#include <queue>

#include "binary_tree.h"

namespace tools
{

template<typename T>
using BtNode = typename BinaryTree<T>::Node;

// 回旋广度遍历
template<typename T = char>
void level_order_retrace(BtNode<T>* ptr)
{
    if (!ptr) return;

    // 使用两个堆栈，交替打印
    Stack<BtNode<T>*> stack2;
    Stack<BtNode<T>*> stack;
    stack.push(ptr);
    bool is_retrace = false;
    while(true)
    {
        if (stack.empty() && stack2.empty()) break;

        if (!is_retrace)
        {
            size_t size = stack.size();
            while(size > 0)
            {
                auto t_ptr = stack.top();
                stack.pop();

                stream << t_ptr->data_ << " ";

                if (t_ptr->left_tree_)
                    stack2.push(t_ptr->left_tree_);
                if (t_ptr->right_tree_)
                    stack2.push(t_ptr->right_tree_);
                
                size --;
            }
        }
        else
        {
            size_t size = stack2.size();
            while(size > 0)
            {
                auto t_ptr = stack2.top();
                stack2.pop();

                stream << t_ptr->data_ << " ";
                // 如果下面一层是逆向打印，则这里需要先右后左地入栈
                if (t_ptr->right_tree_)
                    stack.push(t_ptr->right_tree_);
                if (t_ptr->left_tree_)
                    stack.push(t_ptr->left_tree_);
                
                size --;
            }
        }
        is_retrace = !is_retrace;

        stream << " ";
    }
    stream << std::endl;
}

// bool Is_Full_Binary_Tree(BtNode<T> *ptr);
// bool Is_Comp_Binary_Tree(BtNode *ptr);
// BtNode * FindComParent(BtNode *ptr, BtNode *child1,BtNode *child2);
// int MaxPath(BtNode *ptr, BtNode *&p1,BtNode *&p2);
// void PrintCompBTree(BtNode *ptr, int k);
// int GetBinaryTreeLevel(BtNode *ptr, int k);
// int IsBalance_BinaryTree(BtNode *ptr);
// void BinaryTreeToList(BtNode *ptr);  

}

#endif