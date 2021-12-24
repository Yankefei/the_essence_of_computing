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

// 2 ^ k
size_t power(size_t a, int k)
{
    if (k == 1) return a;

    if (k % 2 == 0)
        return power(a*a, k/2);
    else
        return power(a*a, k/2) * a;
}

// 一种判断方法
// 满二叉树 depth k --> 2^k -1 个节点
bool Is_Full_Binary_Tree1(size_t node_size, size_t depth)
{
    if (power(2, depth) - 1 == node_size)
        return true;

    return false;
}

// 另一种判断方法
// 满二叉树，所有节点的子节点数为0或者为2
template<typename T = char>
bool Is_Full_Binary_Tree2(BtNode<T>* ptr)
{
    // 中序遍历一遍二叉树
    bool is_full = true;
    tools::Stack<BtNode<T>*> stack;
    while(ptr || !stack.empty())
    {
        while(ptr)
        {
            stack.push(ptr);
            ptr = ptr->left_tree_;
        }

        ptr = stack.top();
        stack.pop();
        if ((ptr->left_tree_ == nullptr && ptr->right_tree_ != nullptr) ||
            (ptr->left_tree_ != nullptr && ptr->right_tree_ == nullptr))
        {
            is_full = false;
            break;
        }

        ptr = ptr->right_tree_;
    }

    return is_full;
}


// 完全二叉树
template<typename T = char>
bool Is_Comp_Binary_Tree(BtNode<T> *ptr)
{
    if (ptr == nullptr) return false;
    // 遍历一遍二叉树
    bool is_comp = true;
    // 层序遍历
    bool met_end = false;
    tools::NQueue<BtNode<T>*> queue;
    queue.push(ptr);
    while(!queue.empty())
    {
        BtNode<T>* p = queue.front();
        queue.pop();
        // 如果左右子树均非空，继续
        // 如果左子树为空，右子树非空，直接返回false
        // 如果左子树非空，右子树为空，后者左右子树均为空，则判断后续所有的节点是否均为左右子树皆空
        if (met_end)
        {
            if(p->left_tree_ != nullptr || p->right_tree_ != nullptr)
            {
                is_comp = false;
                break;
            }
        }

        if(p->left_tree_ != nullptr && p->right_tree_ != nullptr)
        {
            queue.push(p->left_tree_);
            queue.push(p->right_tree_);
        }
        else if (p->left_tree_ == nullptr && p->right_tree_ != nullptr)
        {
            is_comp = false;
            break;
        }
        else
        {
            met_end = true;
            if (p->left_tree_)
                queue.push(p->left_tree_);
            if (p->right_tree_)
                queue.push(p->right_tree_);
        }
    }

    return is_comp;
}

// 前序遍历
// template<typename T = char>
// void Pre(BtNode<T>* ptr)
// {
//     tools::Stack<BtNode<T>*> stack;
//     stack.push(ptr);
//     while(!stack.empty())
//     {
//         auto p = stack.top();
//         stack.pop();
        
//         // print

//         if(p->left_tree_)
//             stack.push(p->left_tree_);
//         if (p->right_tree_)
//             stack.push(p->right_tree_);
//     }
// }

// 返回公共父节点
template<typename T = char>
BtNode<T> * FindComParent(BtNode<T> *ptr, BtNode<T> *child1, BtNode<T> *child2)
{
    if (child1 == nullptr || child2 == nullptr) return nullptr;
    // 对二叉树使用非递归后遍历的时候，当找到一个元素时，当前栈中就是所有父节点
    // 对这两个元素的所有父节点进行比较，返回最后一个公共的父节点
    tools::Stack<BtNode<T>*> stack;
    tools::Stack<BtNode<T>*> child1_stack;
    tools::Stack<BtNode<T>*> child2_stack;

    BtNode<T>* p1 = ptr;
    BtNode<T>* t_p = nullptr;
    while(ptr || !stack.empty()) // 必须判断ptr, 否则循环无法进入
    {
        while(p1)
        {
            stack.push(p1);
            p1 = p1->left_tree_;
        }

        p1 = stack.top();
        stack.pop();
        if (p1->right_tree_ == nullptr || p1->right_tree_ == t_p)
        {

            if (p1 == child1)
            {
                auto t_stack = stack;
                while(!t_stack.empty())
                {
                    child1_stack.push(t_stack.top());
                    t_stack.pop();
                }
            }
            if (p1 == child2)
            {
                auto t_stack = stack;
                while(!t_stack.empty())
                {
                    child2_stack.push(t_stack.top());
                    t_stack.pop();
                }
            }
            if (!child1_stack.empty() && !child2_stack.empty())
            {
                break;
            }

            t_p = p1;
            p1 = nullptr;
        }
        else
        {
            stack.push(p1);
            p1 = p1->right_tree_;
        }
    }

    BtNode<T>* val = nullptr;
    while(!child1_stack.empty() && !child2_stack.empty())
    {
        if (child1_stack.top() == child2_stack.top())
        {
            val = child1_stack.top();
            child1_stack.pop();
            child2_stack.pop();
        }
        else
        {
            break;
        }
    }

    return val;
}

// 二叉树的最长路径
template<typename T = char>
int MaxPath(BtNode<T> *ptr, BtNode<T> *&p1, BtNode<T> *&p2)
{
    // 先建立所有叶子节点的缓存信息
    
    return 0;
}

// 打印完全二叉树前几个值
// void PrintCompBTree(BtNode *ptr, int k);

// int GetBinaryTreeLevel(BtNode *ptr, int k);

// int IsBalance_BinaryTree(BtNode *ptr);

// void BinaryTreeToList(BtNode *ptr);  

}

#endif