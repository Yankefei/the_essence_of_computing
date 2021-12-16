#ifndef _TOOLS_BINARY_TREE_H_
#define _TOOLS_BINARY_TREE_H_

#include <memory>
#include <cassert>

#include "stream.h"
#include "tree_util.h"

namespace tools
{

enum class CreateType : uint8_t
{
    PreOrder,
    InOrder,
    PastOrder
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

    }

    void SetEndVal(const T& val) { end_ = val; }

    void CreateTree(T* str, CreateType type)
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
                case CreateType::PastOrder:
                {
                    node = build_tree_past_order(str);
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

    void PrintTree(CreateType type)
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
            case CreateType::PastOrder:
            {
                PastOrder(root_);
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

private:
    Node* buy_node(const T& val = T())
    {
        Node* ptr = new Node(val);
        return ptr;
    }

    void free_node(Node* ptr)
    {
        delete ptr;
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
        return nullptr;
    }

    Node* build_tree_past_order(T*& str)
    {
        return nullptr;
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

    void InOrder(Node* ptr)
    {

    }

    void PastOrder(Node* ptr)
    {

    }

private:
    Root  root_{nullptr};
    T     end_;
};


}

#endif