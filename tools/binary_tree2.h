#ifndef _TOOLS_BINARY_TREE_HPP_
#define _TOOLS_BINARY_TREE_HPP_

#include <memory>
#include <cassert>

#include "stream.h"
#include "stack.h"
#include "queue.h"

#include "binary_tree.h"

namespace tools
{

template<typename T = char>
class BinaryTree2
{
public:
    typedef  _Node<T>*      Root;
    typedef  _Node<T>       Node;

public:
    BinaryTree2() = default;

    BinaryTree2(const T& val) : end_(val) {}

    ~BinaryTree2()
    {
        destory_tree(root_);
    }

    BinaryTree2(const BinaryTree2& val)
    {
        root_ = copy(val.root_);
    }

    BinaryTree2& operator=(const BinaryTree2& val)
    {
        if (this != &val)
        {
            destory_tree(root_);
            root_ = copy(val.root_);
            end_ = val.end_;
        }
    }

    // static bool operator==(const BinaryTree2& lhs, const BinaryTree2& rhs)
    // {
    //     return true;
    // }

    // static bool operator!=(const BinaryTree2& lhs, const BinaryTree2& rhs)
    // {
    //     return true;
    // }

public:
    void set_end_val(const T& val) { end_ = val; }

    Root get_root()
    {
        return root_;
    }

    // 使用输入的完整字符串来创建树
    void create_tree(T* str, OrderType type);

    void print_tree(OrderType type);

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
        destory_tree(root_);
    }

private:
    Node* copy(Node* ptr)
    {
        if (nullptr == ptr) return ptr;

        Node* new_ptr = buy_node(ptr->data_);
        new_ptr->left_tree_ = copy(ptr->left_tree_);
        new_ptr->right_tree_ = copy(ptr->right_tree_);
        return new_ptr;
    }

    static bool is_equal(const BinaryTree2& lhs, const BinaryTree2& rhs)
    {

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

private:
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

    void nice_pre_order(Node*);
    void nice_in_order(Node*);
    void nice_last_order(Node*);
    void level_order(Node*);

private:
    Root  root_{nullptr};
    T     end_;
};

template<typename T>
void BinaryTree2<T>::create_tree(T* str, OrderType type)
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
void BinaryTree2<T>::print_tree(OrderType type)
{
    switch(type)
    {
        case OrderType::PreOrder:
        {
            nice_pre_order(root_);
            break;
        }
        case OrderType::InOrder:
        {
            nice_in_order(root_);
            break;
        }
        case OrderType::LastOrder:
        {
            nice_last_order(root_);
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