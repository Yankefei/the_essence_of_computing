#ifndef _TOOLS_BINARY_TREE_H_
#define _TOOLS_BINARY_TREE_H_

#include <memory>
#include <cassert>

#include "stream.h"
#include "stack.h"
#include "queue.h"

namespace tools
{

enum class OrderType : uint8_t
{
    PreOrder,
    InOrder,
    LastOrder,
    LevelOrder
};

template<typename T = char>
struct _Node
{
    _Node() = default;
    _Node(const T&val) : data_(val) {}

    _Node* left_tree_{nullptr};
    _Node* right_tree_{nullptr};
    T data_;
};

template<typename T = char>
class BinaryTree
{
public:
    typedef  _Node<T>*  Root;

    typedef  _Node<T>   Node;

public:
    BinaryTree() = default;

    BinaryTree(const T& val) : end_(val) {}

    ~BinaryTree()
    {
        destory_tree(root_);
    }

    void set_end_val(const T& val) { end_ = val; }

    Root get_root()
    {
        return root_;
    }

    // 使用输入的完整字符串来创建树
    void create_tree(T* str, OrderType type);

    void print_tree(OrderType type);

    void create_tree_pre_in(T* pre, T* in, int n)
    {
        root_ = build_tree_pre_in(pre, in , n);
    }

    void create_tree_in_last(T* in, T* last, int n)
    {
        root_ = build_tree_in_last(in, last, n);
    }

    bool is_empty()
    {
        return root_ == nullptr;
    }

    size_t size()
    {
        return size_tree(root_);
    }

    size_t max_deep_len()
    {
        return deep_len(root_);
    }

    void clean()
    {
        //destory(root_);
        destory_tree(root_);
    }

    bool find_val(const T& x, Node*& ptr)
    {
        ptr = nullptr;
        ptr = find_val(root_, x);

        return ptr != nullptr;
    }

    bool find_parent(const T& val, T& p)
    {
        Node *ptr = parent(root_, val);

        if (ptr)
        {
            p = ptr->data_;
            return true;
        }

        return false;
    }

public:
    // 另一种堆栈的遍历算法
    // 因为中序遍历和后序遍历都有这样的特点：
    // 中序遍历，一个节点，从上往下第二次遍历的时候，才能遍历到
    // 后序遍历，一个节点，从上往下第三次遍历的时候，才能遍历到
    // 那么可以使用这样的规律，通过出栈的次数来进行打印
    struct StkNode
    {
        Node*   pnode;
        int     popnum;  // 出栈的次数
    public:
        StkNode(Node *p=nullptr):pnode(p),popnum(0){}
    };

    void StkNicePastOrder(Node *ptr)
    {
        if(ptr == nullptr) return ;
        Stack<StkNode > st;
        st.push(StkNode(ptr));
        while(!st.empty())
        {
            StkNode node = st.top();
            st.pop();
            if(++node.popnum == 3) // 第三次出栈的时候才进行打印
            {
                stream <<node.pnode->data_<<"  ";
            }
            else
            {
                st.push(node);
                if(node.popnum == 1 && node.pnode->left_tree_ != nullptr)
                {
                    st.push(StkNode(node.pnode->left_tree_));
                }else if(node.popnum == 2 && node.pnode->right_tree_ != nullptr)
                {
                    st.push(StkNode(node.pnode->right_tree_));
                }
            }
        }
        stream << std::endl;
    }

    void StkNiceInOrder(Node *ptr)
    {
        if(ptr == nullptr) return ;
        Stack<StkNode > st;
        st.push(StkNode(ptr));
        while(!st.empty())
        {
            StkNode node = st.top();
            st.pop();
            if(++node.popnum == 2) // 第二次出栈的时候才进行打印
            {
                if(node.pnode->right_tree_ != nullptr)
                {
                    st.push(StkNode(node.pnode->right_tree_));
                }
                stream <<node.pnode->data_<<"  ";
            }else 
            {
                st.push(node);
                if(node.popnum == 1 && node.pnode->left_tree_ != nullptr)
                {
                    st.push(StkNode(node.pnode->left_tree_));
                }
            }
        
        }
        stream << std::endl;
    }

private:
    Node* parent(Node* ptr, const T& child)
    {
        if (nullptr == ptr) return ptr;

        if ((nullptr != ptr->left_tree_ && child == ptr->left_tree_->data_) ||
            (nullptr != ptr->right_tree_ && child == ptr->right_tree_->data_))
        {
            return ptr;
        }

        auto find = parent(ptr->left_tree_, child);
        if (find == nullptr)
        {
            find = parent(ptr->right_tree_, child);
        }
        return find;
    }

    // alone
    Node* parent_alone(Node* ptr, Node* child)
    {
        if (nullptr == ptr) return ptr;

        if (ptr->left_tree_ == child ||
            ptr->right_tree_ == child)
        {
            return ptr;
        }

        auto find = parent(ptr->left_tree_, child);
        if (find == nullptr)
        {
            find = parent(ptr->right_tree_, child);
        }
        return find;
    }

    Node* find_val(Node* ptr, const T& x)
    {
        if (nullptr == ptr || ptr->data_ == x) return ptr;

        auto find = find_val(ptr->left_tree_, x);
        if (nullptr == find)
        {
            find = find_val(ptr->right_tree_, x);
        }
        return find;
    }

    size_t deep_len(Node* ptr)
    {
        if (ptr == nullptr)
            return 0;
        else
            return std::max(deep_len(ptr->left_tree_),
                            deep_len(ptr->right_tree_)) + 1;
    }

    size_t size_tree(Node* ptr)
    {
        if (ptr == nullptr) return 0;

        return size_tree(ptr->left_tree_) + size_tree(ptr->right_tree_) + 1;
    }

    Root build_tree_pre_in(T* pre, T* in, int n)
    {
        Node* ptr = nullptr;
        // pre != nullptr && in != nullptr  only for safe
        if (n > 0 && pre != nullptr && in != nullptr)
        {
            ptr = buy_node(*pre);
            auto head_index = find_elem(in, *pre, n);
            if (-1 == head_index)
            {
                throw std::runtime_error("invalid parmar in build_tree_pre_in");
            }
            ptr->left_tree_ = build_tree_pre_in(pre+1, in, head_index);
            ptr->right_tree_ =
                build_tree_pre_in(pre + head_index + 1, in + head_index + 1,
                                  n - head_index - 1);
        }
        return ptr;
    }

    Root build_tree_in_last(T* in, T* last, int n)
    {
        Node* ptr = nullptr;
        if (n > 0 && in != nullptr && last != nullptr)
        {
            ptr = buy_node(last[n - 1]);
            auto head_index = find_elem(in, last[n - 1], n);
            if (-1 == head_index)
            {
                throw std::runtime_error("invalid parmar in build_tree_in_last");
            }
            ptr->left_tree_ = build_tree_in_last(in, last, head_index);
            ptr->right_tree_ =
                build_tree_in_last(in + head_index + 1, last + head_index, n - head_index - 1);
        }
        return ptr;
    }

    // 返回元素在当前字符串的第几个位置, 从0统计
    int find_elem(T* str, T val, int n)
    {
        for (int i = 0; i < n; i ++)
        {
            if (str[i] == val) return i;
        }
        return -1;
    }

    Node* buy_node(const T& val = T())
    {
        Node* ptr = new Node(val);
        return ptr;
    }

    void free_node(Node* ptr)
    {
        delete ptr;
    }

    void destory_tree(Node*& ptr)
    {
        // 从下往上删除
        if (!ptr) return;

        destory_tree(ptr->left_tree_);
        destory_tree(ptr->right_tree_);

        free_node(ptr);
        ptr = nullptr;
    }

    void destory(Node*& ptr)
    {
        // 从上往下删除
        if (!ptr) return;

        Node* left = ptr->left_tree_;
        Node* right = ptr->right_tree_;

        free_node(ptr);
        ptr = nullptr;

        destory(left);
        destory(right);
    }

    Node* build_tree_pre_order(T*& str)
    {
        Node* ptr = nullptr;
        if (*str != end_)
        {
            ptr = buy_node(*str);
            ptr->left_tree_ = build_tree_pre_order(++str);
            ptr->right_tree_ = build_tree_pre_order(++str);
        }
        return ptr;
    }

    Node* build_tree_in_order(T*& str)
    {
        Node* ptr = nullptr;
        if (*str != end_)
        {
            ptr->left_tree_ = build_tree_in_order(++str);
            ptr = buy_node(*str);
            ptr->right_tree_ = build_tree_in_order(++str);
        }
        return ptr;
    }

    Node* build_tree_last_order(T*& str)
    {
        Node* ptr = nullptr;
        if (*str != end_)
        {
            ptr->left_tree_ = build_tree_last_order(++str);
            ptr->right_tree_ = build_tree_last_order(++str);
            ptr = buy_node(*str);
        }
        return ptr;
    }

    void PreOrder(Node* ptr)
    {
        if (ptr != nullptr)
        {
            stream << ptr->data_ << " ";
            PreOrder(ptr->left_tree_);
            PreOrder(ptr->right_tree_);
        }
    }

    void my_nice_pre_order(Node* ptr);

    // 非递归的前序遍历
    void nice_pre_order(Node* ptr)
    {
        if (!ptr) return;
        Stack<Node*> stack;
        stack.push(ptr);

        while(!stack.empty())
        {
            auto t_ptr = stack.top();
            stack.pop();
            stream << t_ptr->data_ << " ";
            
            if (t_ptr->right_tree_)
            {
                stack.push(t_ptr->right_tree_);
            }
            if (t_ptr->left_tree_)
            {
                stack.push(t_ptr->left_tree_);
            }
        }
    }

    void InOrder(Node* ptr)
    {
        if (ptr != nullptr)
        {
            InOrder(ptr->left_tree_);
            stream << ptr->data_ << " ";
            InOrder(ptr->right_tree_);
        }
    }

    void my_nice_in_order(Node* ptr);

    // 非递归的中序遍历
    void nice_in_order(Node* ptr)
    {
        if (!ptr) return;
        Stack<Node*> stack;
        while(ptr || !stack.empty())
        {
            while(ptr)
            {
                stack.push(ptr);
                ptr = ptr->left_tree_;
            }
            ptr = stack.top();
            stack.pop();
            stream << ptr->data_ << " ";
            ptr = ptr->right_tree_;
        }
    }

    void LastOrder(Node* ptr)
    {
        if (ptr != nullptr)
        {
            LastOrder(ptr->left_tree_);
            LastOrder(ptr->right_tree_);
            stream << ptr->data_ << " ";
        }
    }

    void my_nice_last_order(Node* ptr);

    // 非递归后序遍历
    void nice_last_order(Node* ptr)
    {
        if (!ptr) return;
        Stack<Node*> stack;
        Node* f_ptr = nullptr;  // 一个记录指针，时刻记录着已经遍历的最后一个节点
        while(ptr || !stack.empty())
        {
            while(ptr)
            {
                stack.push(ptr);
                ptr = ptr->left_tree_;
            }

            ptr = stack.top();
            stack.pop();
            if (ptr->right_tree_ == nullptr  ||
                ptr->right_tree_ == f_ptr)
            {
                stream << ptr->data_ << " ";
                f_ptr = ptr;             // 记录指针赋值
                ptr = nullptr;           // 每次将指针置空, 很关键已经后面需要出栈
            }
            else
            {
                stack.push(ptr);  // 重新入栈
                ptr = ptr->right_tree_;
            }
        }
    }

    void level_order(Node* ptr)
    {
        if (!ptr) return;

        tools::Queue<Node*> queue;
        queue.push_back(ptr);
        while(!queue.empty())
        {
            auto t_ptr = queue.front();
            queue.pop_front();

            stream << t_ptr->data_ << " ";
            if (t_ptr->left_tree_)
                queue.push_back(t_ptr->left_tree_);
            if (t_ptr->right_tree_)
                queue.push_back(t_ptr->right_tree_);
        }
    }

private:


private:
    Root  root_{nullptr};
    T     end_;
};

template<typename T>
void BinaryTree<T>::my_nice_pre_order(Node* ptr)
{
    if (!ptr) return;

    Node* f_ptr = ptr; // 遍历伴随指针
    tools::Stack<Node*> stack;
    while(true)
    {
        while(f_ptr->left_tree_ || f_ptr->right_tree_)
        {
            if (f_ptr->right_tree_)
            {
                stack.push(f_ptr->right_tree_);
            }

            stream << f_ptr->data_ << " "; // 非叶子节点

            if (f_ptr->left_tree_)
            {
                f_ptr = f_ptr->left_tree_;
            }
            else
            {
                break;
            }
        }

        if (f_ptr->right_tree_ == nullptr && f_ptr->left_tree_ == nullptr)
            stream << f_ptr->data_ << " "; // 叶子结点

        if (!stack.empty())
        {
            f_ptr = stack.top();
            stack.pop();
        }
        else
        {
            break;
        }
    }
}

/*
    写代码的每一行都要有依据，决不能让过程成为黑盒子，因为只有每一步都有依据
    才能不断做到迭代提升
*/
// template<typename T>
// void BinaryTree<T>::my_nice_in_order(Node* ptr)
// {
//     if (!ptr) return;

//     Node* f_ptr = ptr;   // 遍历伴随指针
//     Node* p_ptr = nullptr; // 记录当前遍历指针的父节点
//     tools::Stack<Node*> stack;
//     while(true)
//     {
//         if (p_ptr) // 只要从堆栈中取出，直接使用不需要再判断, 一定是f_ptr的父节点
//         {
//             f_ptr = p_ptr; // p_ptr 更新f_ptr
//             p_ptr = nullptr; // 清理
//             stream << f_ptr->data_ << " ";  // 打印子树的根节点
//         }
//         else
//         {
//             while(nullptr != f_ptr->left_tree_)
//             {
//                 stack.push(f_ptr);
//                 f_ptr = f_ptr->left_tree_;
//             }

//             stream << f_ptr->data_ << " ";  // 左子树为空的节点
//         }

//         if (f_ptr->right_tree_)
//         {
//             f_ptr = f_ptr->right_tree_;
//             continue;
//         }

//         if (!stack.empty())
//         {
//             p_ptr = stack.top();
//             stack.pop();
//         }
//         else
//         {
//             break;
//         }
//     }
// }

template<typename T>
void BinaryTree<T>::my_nice_in_order(Node* ptr)
{
    // Node* p = ptr; // 使用一个指针记录当前的根节点
    // tools::Stack<Node*> stack;

    // while(p || !stack.empty())
    // {
    //     if (p)
    //     {
    //         stack.push(p);
    //         p = p->left_tree_;  
    //     }
    //     else
    //     {
    //         p = stack.top();
    //         stack.pop();
    //         stream << p->data_ << " ";
    //         p = p->right_tree_;
    //     }
    // }

    tools::Stack<Node*> stack;
    Node* p = nullptr;
    stack.push(ptr);
    while(!stack.empty())
    {
        while(!stack.empty() && (p = stack.top()))
        {
            stack.push(p->left_tree_);
        }
        stack.pop();

        if (!stack.empty())
        {
            p = stack.top();
            stack.pop();
            stream << p->data_ << " ";
            stack.push(p->right_tree_);
        }
    }
}

template<typename T>
void BinaryTree<T>::my_nice_last_order(Node* ptr)
{
    if (!ptr) return;

    Node* f_ptr = ptr;
    Node* p_ptr = nullptr;
    tools::Stack<Node*> stack;
    while(true)
    {
        if (p_ptr)
        {
            if (p_ptr->right_tree_)
            {
                if (f_ptr != p_ptr->right_tree_)
                {
                    f_ptr = p_ptr->right_tree_;
                    p_ptr = nullptr;
                    continue;
                }
                else
                {
                    stream << p_ptr->data_ << "1 "; //
                    f_ptr = p_ptr;
                    p_ptr = nullptr;
                }
            }
            else
            {
                stream << p_ptr->data_ << "2 "; //
                f_ptr = p_ptr;
                p_ptr = nullptr;
            }
        }
        else
        {
            while(f_ptr->left_tree_)
            {
                stack.push(f_ptr);
                f_ptr = f_ptr->left_tree_;
            }

            if (f_ptr->right_tree_)
            {
                stack.push(f_ptr);
                f_ptr = f_ptr->right_tree_;
                continue;
            }
            else
            {
                stream << f_ptr->data_ << "3 ";  // 左子树为空的节点
            }
        }

        if (!stack.empty())
        {
            p_ptr = stack.top();
            // 如果右子树没有遍历过，则遍历，否则直接出栈
            if (p_ptr->right_tree_ != nullptr && f_ptr != p_ptr->right_tree_)
            {
                f_ptr = p_ptr->right_tree_; // 出栈一个右子树
                p_ptr = nullptr;
            }
            else
            {
                stack.pop();
            }
        }
        else
        {
            break;
        }
    }
}

template<typename T>
void BinaryTree<T>::create_tree(T* str, OrderType type)
{
    if (str != nullptr)
    {
        Root node = nullptr;
        switch(type)
        {
            case OrderType::PreOrder:
            {
                node = build_tree_pre_order(str);
                break;
            }
            case OrderType::InOrder:
            {
                node = build_tree_in_order(str);
                break;
            }
            case OrderType::LastOrder:
            {
                node = build_tree_last_order(str);
                break;
            }
            default:
            {
                assert(false);
                break;
            }
        }
        root_ = node;
    }
}

template<typename T>
void BinaryTree<T>::print_tree(OrderType type)
{
    switch(type)
    {
        case OrderType::PreOrder:
        {
            nice_pre_order(root_);
            //PreOrder(root_);
            break;
        }
        case OrderType::InOrder:
        {
            my_nice_in_order(root_);
            //nice_in_order(root_);
            //InOrder(root_);
            break;
        }
        case OrderType::LastOrder:
        {
            nice_last_order(root_);
            //LastOrder(root_);
            break;
        }
        case OrderType::LevelOrder:
        {
            level_order(root_);
            break;
        }
        default:
        {
            assert(false);
            break;
        }
    }
    stream << std::endl;
}

}

#endif