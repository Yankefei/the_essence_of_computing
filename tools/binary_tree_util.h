#ifndef _TOOLS_BINARY_TREE_UTIL_H_
#define _TOOLS_BINARY_TREE_UTIL_H_

#include <memory>
#include <cassert>

#include "vector.hpp"

#include "binary_tree.h"

namespace tools
{
template<typename T = char>
using BtNode = _Node<T>;

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
    if (k == 0) return 1;
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
    while(p1 || !stack.empty()) // 必须判断ptr, 否则循环无法进入
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
int MaxPath1(BtNode<T> *ptr, BtNode<T> *&p1, BtNode<T> *&p2)
{
    // 先建立所有叶子节点到根节点的缓存信息
    tools::Vector<tools::Vector<BtNode<T>*>> cache_info;

    size_t max_depth = 0; // 记录所有缓存信息的最大值

    // 后序遍历二叉树
    BtNode<T>* p = ptr;
    BtNode<T>* last = nullptr;
    tools::Stack<BtNode<T>*> stack;
    while(p || !stack.empty())
    {
        while(p)
        {
            stack.push(p);
            p = p->left_tree_;
        }

        p = stack.top();
        stack.pop();
        if (p->right_tree_ == nullptr ||
            p->right_tree_ == last)
        {
            tools::Vector<BtNode<T>*> temp_v;
            temp_v.emplace_back(p);
            tools::Stack<BtNode<T>*> t_stack = stack;
            while(!t_stack.empty())
            {
                // 从叶子节点到根节点的顺序保存
                temp_v.emplace_back(t_stack.top());
                t_stack.pop();
            }

            max_depth = std::max(max_depth, temp_v.size());
            cache_info.emplace_back(std::move(temp_v));
            last = p;
            p = nullptr;
        }
        else
        {
            stack.push(p);
            p = p->right_tree_;
        }
    }

    // stream << "size: " << cache_info.size() << std::endl;

    // 最大距离就是两个叶子结点直接的最大距离
    int max_path = 0;
    BtNode<T>* _p1 = nullptr;
    BtNode<T>* _p2 = nullptr;
    for (int i = 0; i < cache_info.size(); i ++)
    {
        for (int j = i + 1; j < cache_info.size(); j++)
        {
            auto& v1 = cache_info[i];
            auto& v2 = cache_info[j];

             // 如果两边的长度之和小于最大长度，那么直接排除
            if (v1.size() + v2.size() < max_depth) continue;

            int v1_len = v1.size();
            int v2_len = v2.size();
            while(v1_len-- > 0 && v2_len-- > 0)
            {
                if (v1[v1_len] == v2[v2_len])
                    continue;
                else
                {
                    int t_max_path = v1_len + v2_len + 2; 
                    if (t_max_path > max_path)
                    {
                        max_path = t_max_path;
                        _p1 = v1[0];
                        _p2 = v2[0];
                    }
                    break;
                }
            }
        }
    }

    p1 = _p1;
    p2 = _p2;

    return max_path;
}

struct PathInfo
{
    int max_depth = 0;
    int max_path = 0;
};

template<typename T = char>
PathInfo max_path_2_(BtNode<T>* ptr)
{
    if (ptr == nullptr)
    {
        PathInfo empty;
        empty.max_depth = -1;
        empty.max_path = 0;
        return empty;
    }

    PathInfo left = max_path_2_(ptr->left_tree_);
    PathInfo right = max_path_2_(ptr->right_tree_);

    PathInfo res;
    res.max_depth = std::max(left.max_depth + 1, right.max_depth + 1);
    res.max_path = std::max((left.max_path, right.max_path),
                            left.max_depth + right.max_depth + 2);
    
    return res;
}

template<typename T = char>
PathInfo max_path_2(BtNode<T>* ptr)
{
    PathInfo res;
    res.max_depth = -1;

    PathInfo left;
    left.max_depth = -1;
    if (ptr->left_tree_)
    {
        left = max_path_2(ptr->left_tree_);
    }

    PathInfo right;
    right.max_depth = -1;
    if (ptr->right_tree_)
    {
        right = max_path_2(ptr->right_tree_);
    }

    res.max_depth = std::max(left.max_depth + 1, right.max_depth + 1);

    // 关键，每一层的max_path都不一样
    // 不要试答案， 要调试
    // 这个地方是核心逻辑
    res.max_path = std::max(std::max(left.max_path, right.max_path),
                                     left.max_depth + right.max_depth + 2);

    return res;
}

// 二叉树的最长路径
template<typename T = char>
size_t MaxPath2(BtNode<T> *ptr)
{
    if (!ptr) return 0;

    // 经过分析后发现，这里的初始值设置地非常巧妙, 是需要提前思考算法么？
    //return max_path_2_(ptr).max_path;
    return max_path_2(ptr).max_path;
}


template<typename T = char>
struct PathNode
{
    BtNode<T>* path_old_t = nullptr; // old的节点更接近叶子节点
    BtNode<T>* path_new_t = nullptr; // new的节点是后面遍历的节点
};


// Using 设置别名的时候如果内部类型还有模板，需要在模板列表中定义
// 用递归的方式返回指针的值，实在有点太偏，等后面有时间了再来回看，目前先跳过
// 至少成功实验了 using 设置模板别名的用法，还是很不错的
// TODO
template<typename T, typename P = PathNode<T>>
using Result = std::pair<PathInfo, P>;  

template<typename T = char>
Result<T> max_path_3(BtNode<T>* ptr)
{
    Result<T> re;
    re.first.max_depth = -1;

    Result<T> l_info;
    l_info.first.max_depth = -1;
    if(ptr->left_tree_)
        l_info = max_path_3(ptr->left_tree_);
    else
    {
        // 子树为空的节点需要初始化记录指针
        l_info.second.path_old_t = ptr;
        l_info.second.path_new_t = ptr;
    }


    Result<T> r_info;
    r_info.first.max_depth = -1;
    if (ptr->right_tree_)
        r_info = max_path_3(ptr->right_tree_);
    else
    {
        // 子树为空的节点需要初始化记录指针
        r_info.second.path_old_t = ptr;
        r_info.second.path_new_t = ptr;
    }

    // 靠逻辑而不是找规律
    re.first.max_depth = std::max(l_info.first.max_depth + 1, r_info.first.max_depth + 1);

    size_t comm_path = l_info.first.max_depth + r_info.first.max_depth + 2;
    if (l_info.first.max_path > r_info.first.max_path)
    {
        if (comm_path > l_info.first.max_path)
        {
            // 需要融合两个边的分支
            re.first.max_path = comm_path;

            // 融合时, 根据最大深度来判断哪一个边是old指针，哪一个边为new指针
            if(l_info.first.max_depth > r_info.first.max_depth)
            {
                re.second.path_old_t = l_info.second.path_old_t;
                re.second.path_new_t = r_info.second.path_old_t;
            }
            else
            {
                re.second.path_old_t = r_info.second.path_old_t;
                re.second.path_new_t = l_info.second.path_old_t;
            }
        }
        else
        {
            // 选择一边的最大距离
            re.first.max_path = l_info.first.max_path;

            re.second.path_new_t = l_info.second.path_new_t;
            re.second.path_old_t = l_info.second.path_old_t;
        }
    }
    else
    {
        if (comm_path > r_info.first.max_path)
        {
            // 记录两边旧的节点
            re.first.max_path = comm_path;

            if(l_info.first.max_depth > r_info.first.max_depth)
            {
                re.second.path_old_t = l_info.second.path_old_t;
                re.second.path_new_t = r_info.second.path_old_t;
            }
            else
            {
                re.second.path_old_t = r_info.second.path_old_t;
                re.second.path_new_t = l_info.second.path_old_t;
            }
        }
        else
        {
            re.first.max_path = r_info.first.max_path;

            re.second.path_new_t = r_info.second.path_new_t;
            re.second.path_old_t = r_info.second.path_old_t;
        }
    }

    return re;
}

// 二叉树的最长路径
template<typename T = char>
int MaxPath3(BtNode<T> *ptr, BtNode<T> *&p1, BtNode<T> *&p2)
{
    if (ptr == nullptr) return 0;
    
    auto result = max_path_3(ptr);
    p1 = result.second.path_new_t;
    p2 = result.second.path_old_t;

    // stream <<"max_path: " <<  result.first.max_path <<
    //         " " << result.second.path_new_t <<
    //         " " << result.second.path_old_t<< std::endl;

    return result.first.max_path;
}

// 使用数组方式保存的二叉树
template<typename T = char>
void Link(BtNode<T>* nodes, int parent, int left, int right)
{
    if (left != -1)
        nodes[parent].left_tree_ = &nodes[left];
    
    if (right != -1)
        nodes[parent].right_tree_ = &nodes[right];
}


// 打印完全二叉树第k层
template<typename T = char>
void PrintCompBTree(BtNode<T> *ptr, int k)
{
    if (!ptr || k < 1) return;

    // 左开右闭区间
    int print_begin_index = power(2, k - 1) - 1;
    int print_end_index = power(2, k) - 1;

    int index = 1;

    tools::Queue<BtNode<T>*> queue;
    queue.push(ptr);
    while(!queue.empty())
    {
        ptr = queue.front();
        queue.pop_front();

        if (index > print_begin_index && index <= print_end_index) 
        {
            stream << ptr->data_ << " ";
            if (index == print_end_index)
                break;
        }
        index ++;

        if (ptr->left_tree_)
            queue.push_back(ptr->left_tree_);

        if (ptr->right_tree_)
            queue.push_back(ptr->right_tree_);
    }

    stream << std::endl;
}

// 返回第一个遇到的层节点数为k的层数
template<typename T = char>
int GetBinaryTreeLevel(BtNode<T> *ptr, int k)
{
    if (!ptr) return -1;

    int level = 1;

    tools::Queue<BtNode<T>*> queue1;
    tools::Queue<BtNode<T>*> queue2;
    queue1.push(ptr);
    while(!queue1.empty() || !queue2.empty())
    {
        if (queue1.size() == k)
            break;

        while(!queue1.empty())
        {
            ptr = queue1.front();
            queue1.pop_front();

            if (ptr->left_tree_)
                queue2.push_back(ptr->left_tree_);

            if (ptr->right_tree_)
                queue2.push_back(ptr->right_tree_);
        }
        level ++;
        if (queue2.size() == k)
            break;

        while(!queue2.empty())
        {
            ptr = queue2.front();
            queue2.pop_front();

            if (ptr->left_tree_)
                queue1.push_back(ptr->left_tree_);

            if (ptr->right_tree_)
                queue1.push_back(ptr->right_tree_);
        }
        level ++;
    }

    return level;
}

// 是否为平衡树
/*
    满足以下两点的就是平衡二叉树:
    1.左右子树的高度差不能超过1
    2.左右子树也是平衡二叉树

    空树也是平衡二叉树
*/
// 关键点：递归函数需要同时返回左右子树的高度以及左右子树是否为平衡树的bool类型
using BalancePair = std::pair<bool, int>;

template<typename T = char>
BalancePair is_balance_binary_tree(BtNode<T>* ptr)
{
    BalancePair res{true, 0};
    BalancePair left_flag;
    BalancePair right_flag;

    if (ptr->left_tree_)
        left_flag = is_balance_binary_tree(ptr->left_tree_);
    else
    {
        left_flag = {true, 0};
    }

    if (ptr->right_tree_)
        right_flag = is_balance_binary_tree(ptr->right_tree_);
    else
    {
        right_flag = {true, 0};
    }

    res.second = std::max(left_flag.second + 1, right_flag.second +1);
    if (!(left_flag.first && right_flag.first))
    {
        res.first = false;
    }
    else
    {
        if (left_flag.second > right_flag.second)
        {
            if (left_flag.second - right_flag.second > 1)
                res.first = false;
        }
        else
        {
            if (right_flag.second - left_flag.second> 1)
                res.first = false;
        }
    }

    return res;
}

template<typename T = char>
bool IsBalance_BinaryTree(BtNode<T> *ptr)
{
    return is_balance_binary_tree(ptr).first;
}

// 保存到列表中
// 按照二叉树的层序遍历的顺序保存到数组中，数组的组织形式见上面的函数：Link
template<typename T = char>
void BinaryTreeToList(BtNode<T> *ptr, BtNode<T>* dst, size_t size)
{
    if (ptr == nullptr || size == 0) return;

    BtNode<T>* f_dst = dst;
    BtNode<T>* next_level_ptr = nullptr; // 指向下一层二叉树的头部指针
    BtNode<T>* end_ptr = dst + size;     // 二叉数组的尾部边界，超过需要将子树指针置为nullptr

    // 这里使用两个队列，因为需要确切知道每层二叉树的节点个数
    tools::Queue<BtNode<T>*> queue1;
    tools::Queue<BtNode<T>*> queue2;
    queue1.push_back(ptr);
    while(!queue1.empty() || !queue2.empty())
    {
        next_level_ptr = f_dst + queue1.size();
        while(!queue1.empty())
        {
            ptr = queue1.front();
            queue1.pop_front();
            *f_dst = *ptr;

            if (ptr->left_tree_)
            {
                queue2.push_back(ptr->left_tree_);
                if (next_level_ptr >= end_ptr)
                {
                    f_dst->left_tree_ = nullptr;
                }
                else
                    f_dst->left_tree_ = next_level_ptr ++;
            }
            else
                f_dst->left_tree_ = nullptr;

            if (ptr->right_tree_)
            {
                queue2.push_back(ptr->right_tree_);
                if (next_level_ptr >= end_ptr)
                {
                    f_dst->right_tree_ = nullptr;
                }
                else
                    f_dst->right_tree_ = next_level_ptr ++;
            }
            else
                f_dst->right_tree_ = nullptr;
            
            f_dst ++;
        }

        next_level_ptr = f_dst + queue2.size();
        while(!queue2.empty())
        {
            ptr = queue2.front();
            queue2.pop_front();
            *f_dst = *ptr;

            if (ptr->left_tree_)
            {
                queue1.push_back(ptr->left_tree_);
                if (next_level_ptr >= end_ptr)
                {
                    f_dst->left_tree_ = nullptr;
                }
                else
                    f_dst->left_tree_ = next_level_ptr ++;
            }
            else
                f_dst->left_tree_ = nullptr;

            if (ptr->right_tree_)
            {
                queue1.push_back(ptr->right_tree_);
                if (next_level_ptr >= end_ptr)
                {
                    f_dst->right_tree_ = nullptr;
                }
                else
                    f_dst->right_tree_ = next_level_ptr ++;
            }
            else
                f_dst->right_tree_ = nullptr;

            f_dst ++;
        }
    }
}

// 遍历 层序建立的完全二叉树数组
void PreOrder(char *ar,int i,int n)
{
	if(ar != NULL && i<n)
	{
		stream << ar[i] << " ";
		PreOrder(ar,i*2+1,n);
		PreOrder(ar,i*2+2,n);
	}
}

void InOrder(char *ar,int i,int n)
{
	if(ar != NULL && i<n)
	{
		InOrder(ar,i*2+1,n);
		stream << ar[i] << " ";
		InOrder(ar,i*2+2,n);
	}
}

void LastOrder(char *ar,int i,int n)
{
	if(ar != NULL && i<n)
	{
		LastOrder(ar,i*2+1,n);
		LastOrder(ar,i*2+2,n);
		stream << ar[i] << " ";
	}
}

}

#endif