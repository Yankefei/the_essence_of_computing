#ifndef _TOOLS_BINARY_TREE_H_
#define _TOOLS_BINARY_TREE_H_

#include <memory>
#include <cassert>

#include "stream.h"
#include "stack.h"
#include "tree_util.h"

namespace tools
{

enum class CreateType : uint8_t
{
    PreOrder,
    InOrder,
    LastOrder
};

template<typename T = char>
class BinaryTree
{
    struct Node
    {
        Node() = default;
        Node(const T&val) : data_(val) {}

        Node* left_tree_{nullptr};
        Node* right_tree_{nullptr};
        T data_;
    };

    using Root = Node*;

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
    void create_tree(T* str, CreateType type);

    void print_tree(CreateType type);

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

    bool find_val(const T& x)
    {
        Node* ptr = nullptr;
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
                build_tree_pre_in(pre + head_index + 1, in + head_index + 1, n - head_index - 1);
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

    void nice_pre_order(Node* ptr)
    {

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

    void nice_in_order(Node* ptr)
    {

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

    void nice_last_order(Node* ptr)
    {

    }

private:
    Root  root_{nullptr};
    T     end_;
};

template<typename T>
void BinaryTree<T>::create_tree(T* str, CreateType type)
{
    if (str != nullptr)
    {
        Root node = nullptr;
        switch(type)
        {
            case CreateType::PreOrder:
            {
                node = build_tree_pre_order(str);
                break;
            }
            case CreateType::InOrder:
            {
                node = build_tree_in_order(str);
                break;
            }
            case CreateType::LastOrder:
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
void BinaryTree<T>::print_tree(CreateType type)
{
    switch(type)
    {
        case CreateType::PreOrder:
        {
            PreOrder(root_);
            break;
        }
        case CreateType::InOrder:
        {
            InOrder(root_);
            break;
        }
        case CreateType::LastOrder:
        {
            LastOrder(root_);
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