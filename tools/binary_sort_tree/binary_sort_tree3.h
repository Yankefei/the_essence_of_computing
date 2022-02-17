#ifndef _TOOLS_BINARY_SORT_TREE3_H_
#define _TOOLS_BINARY_SORT_TREE3_H_

#include <memory>
#include <cassert>

#include "binary_sort_tree_base.h"
#include "algorithm.hpp"
#include "pair.hpp"

namespace tools
{

template<typename T>
struct _SNode3
{
    _SNode3() = default;
    _SNode3(const T&val) : data_(val) {}

    _SNode3* left_tree_{nullptr};
    _SNode3* right_tree_{nullptr};
    _SNode3* parent_{nullptr};
    T data_;
};

/*排序二叉树*/
template<typename T, template <typename T1> class SNode = _SNode3, typename Alloc = std::allocator<T>>
class BsTree : protected BsTree_Base<T, SNode, Alloc>
{
    typedef BsTree_Base<T, SNode, Alloc> BsTreeBase;

public:
    typedef typename BsTreeBase::Node Node;
    typedef Node*  Root;

public:
    using BsTreeBase::buy_node;
    using BsTreeBase::free_node;
    using BsTreeBase::_m_impl;

public:
    BsTree() {}
    ~BsTree() { destory(_m_impl._root); }

    BsTree(const BsTree& rhs)
    {
        _m_impl._root = copy(rhs._m_impl._root);
    }

    BsTree& operator=(const BsTree& rhs)
    {
        if (this != &rhs)
        {
            destory(_m_impl._root);
            _m_impl._root = copy(rhs._m_impl._root);
        }
        return *this;
    }

    BsTree(BsTree&& rhs)   noexcept
    {
        _m_impl._root = rhs._m_impl._root;
        rhs._m_impl._root = nullptr;
    }

    BsTree& operator=(BsTree&& rhs)   noexcept
    {
        if (this != &rhs)
        {
            _m_impl._root = rhs._m_impl._root;
            rhs._m_impl._root = nullptr;
        }
        return *this;
    }

    // 有一种处理重复数据的方案，就是在数据域增加一个频率的字段
    // 不过在关键字仅仅为数据的一部分这种情况，还必须使用其他的处理方案

    // 插入成功，返回Node节点指针，插入失败有重复数据，也返回对应的指针
    Pair<bool, Node*> c_insert(const T& val)   //用于容器 
    {
        return insert(&_m_impl._root, val);
    }

    bool insert(const T& val)
    {
        auto pair = insert(&_m_impl._root, val);
        return pair.first;
    }

    // 递归版本
    bool insert2(const T& val)
    {
        return insert(_m_impl._root, val, nullptr);
    }

    bool remove(const T& val)
    {
        return remove(&_m_impl._root, val);
    }

    void InOrder() const
    {
        _InOrder(_m_impl._root);
        stream << std::endl;
    }

    size_t NiceInOrder()
    {
        size_t ele_num = 0;
        auto ptr = first();
        Node* min = ptr;
        for (; ptr != nullptr; ptr = next(ptr))
        {
            if (alg::le(ptr->data_, min->data_))
                assert(false);
            //stream << ptr->data_ << " ";
            min = ptr;
            ele_num ++;
        }
        //stream << std::endl;
        return ele_num;
    }

    size_t ResNiceInOrder()
    {
        size_t ele_num = 0;
        auto ptr = last();
        Node* max = ptr;
        for (; ptr != nullptr; ptr = prev(ptr))
        {
            if (alg::le(max->data_, ptr->data_))
                assert(false);
            // stream << ptr->data_ << " ";
            max = ptr;
            ele_num ++;
        }
        // stream << std::endl;
        return ele_num;
    }

    Node* find(const T& val) const
    {
        return find(_m_impl._root, val);
    }

    void clear()
    {
        destory(_m_impl._root);
        _m_impl._root = nullptr;
    }

    Node* get_root()
    {
        return _m_impl._root;
    }

    bool is_same(const BsTree& rhs)
    {
        return check_same(_m_impl._root, rhs._m_impl._root) &&
               check_same2(_m_impl._root, rhs._m_impl._root);
    }

public:
    Node* first()
    {
        return _first(_m_impl._root);
    }

    static Node* next(Node* ptr)
    {
        if (ptr == nullptr) return ptr;

        if (ptr->right_tree_)
        {
            return _first(ptr->right_tree_);
        }
        else
        {
            // 真的不知道这样的代码是怎么写出来的。。服
            Node* pa = ptr->parent_;
            while(pa != nullptr && pa->left_tree_ != ptr)
            {
                ptr = pa;
                pa = pa->parent_;
            }
            return pa;

            // if (ptr->parent_ == nullptr) return nullptr;

            // if (alg::le(ptr->parent_->data_, ptr->data_))
            // {
            //     if (ptr->parent_->parent_ == nullptr) return nullptr;
                
            //     if (alg::le(ptr->parent_->parent_->data_, ptr->data_))
            //     {
            //         return nullptr;
            //     }
            //     else
            //     {
            //         return ptr->parent_->parent_;
            //     }
            // }
            // else
            // {
            //     return ptr->parent_;
            // }
        }
    }

private:
    // 复制一棵树, 返回新的ptr指针, 同时维护parent_指针
    Node* copy(Node* ptr)
    {
        if (nullptr == ptr) return ptr;

        // 先获取ptr树的left节点
        Node* left = ptr;
        while(left->left_tree_)
            left = left->left_tree_;

        Node* new_ptr = nullptr;
        Node* new_ptr_next = nullptr; // new_ptr 的左子树
        for(; left != ptr->parent_; left = left->parent_)
        {
            new_ptr_next = new_ptr;
            new_ptr = buy_node(left->data_);
            new_ptr->right_tree_ = copy(left->right_tree_);
            if (new_ptr->right_tree_ != nullptr)
                new_ptr->right_tree_->parent_ = new_ptr;
            
            if (new_ptr_next != nullptr)
                new_ptr_next->parent_ = new_ptr;
            new_ptr->left_tree_ = new_ptr_next;
        }

        return new_ptr;
    }

    // 递归检查
    bool check_same(Node* ptr, Node* ptr2)
    {
        if (ptr == nullptr && ptr2 == nullptr) return true;

        if (ptr == nullptr || ptr2 == nullptr) return false;

        if (alg::eq(ptr->data_, ptr2->data_) &&
            check_same(ptr->left_tree_, ptr2->left_tree_) &&
            check_same(ptr->right_tree_, ptr2->right_tree_))
        {
            return true;
        }

        return false;
    }

    // 正反遍历检查
    bool check_same2(Node* root1, Node* root2)
    {
        {
            Node* ptr = _first(root1);
            Node* ptr2 = _first(root2);
            for (; ptr != nullptr; ptr = next(ptr), ptr2 = next(ptr2))
            {
                if (alg::neq(ptr->data_, ptr2->data_))
                    return false;
            }
            if (ptr2 != nullptr) return false;
        }

        {
            Node* ptr = _last(root1);
            Node* ptr2 = _last(root2);
            for (; ptr != nullptr; ptr = prev(ptr), ptr2 = prev(ptr2))
            {
                if (alg::neq(ptr->data_, ptr2->data_))
                    return false;
            }
            if (ptr2 != nullptr) return false;
        }
        return true;
    }

    bool insert(Node*& ptr, const T& val, Node* pptr)
    {
        bool res = false;
        if (ptr == nullptr)
        {
            ptr = buy_node(val);
            ptr->parent_ = pptr;
            res = true;
        }
        else if (alg::le(val, ptr->data_))
        {
            return insert(ptr->left_tree_, val, ptr);
        }
        else if (alg::le(ptr->data_, val))
        {
            return insert(ptr->right_tree_, val, ptr);
        }

        return res;
    }

    Pair<bool, Node*> insert(Node** pptr, const T& val)
    {
        if (pptr == nullptr) return {false, nullptr};
        Node* pa = nullptr;
        Node* p = *pptr;
        while(p != nullptr && alg::neq(p->data_, val))
        {
            pa = p;
            p = alg::le(val, p->data_) ? p->left_tree_ : p->right_tree_;
        }
        if (p != nullptr && alg::eq(p->data_, val))
        {
            return {false, p};
        }

        // 在不需要区分左右的时候，先不区分，这样后面代码会好写很多，简洁很多
        // 尤其在下面remove的时候，最为明显
        p = buy_node(val);
        p->parent_ = pa;
        if (pa == nullptr)
            *pptr = p;
        else
        {
            if (alg::le(p->data_, pa->data_))
            {
                pa->left_tree_ = p;
            }
            else
            {
                pa->right_tree_ = p;
            }
        }

        return {true, p};
    }

    // 删除的过程需要维护 parent_ 指针
    bool remove(Node** pptr, const T& val)
    {
        if (pptr == nullptr || *pptr == nullptr) return false;

        Node* ptr = find(*pptr, val);
        if (ptr == nullptr) return false;
        if (ptr->left_tree_ != nullptr && ptr->right_tree_ != nullptr)
        {
            // 找出右子树最小值替换
            Node* p = ptr->right_tree_;
            while(p->left_tree_)
            {
                p = p->left_tree_;
            }
            ptr->data_ = p->data_;
            // 替换之后，ptr就不重要了，处理好p的节点关系即可，所以直接复用变量
            ptr = p;
        }

        // 删除ptr的指针，逻辑一致了
        Node* pa = ptr->parent_;
        Node* child = ptr->left_tree_ == nullptr ? ptr->right_tree_ : ptr->left_tree_;

        if (child != nullptr) child->parent_ = pa;
        // 最后再处理 pa， 真的很妙
        if (pa == nullptr)
        {
            *pptr = child;
        }
        else
        {
            if (pa->left_tree_ == ptr)
            {
                pa->left_tree_ = child;
            }
            else
            {
                pa->right_tree_ = child;
            }
        }

        free_node(ptr);

        return true;
    }

    static void _InOrder(Node* ptr)
    {
        if (ptr)
        {
            _InOrder(ptr->left_tree_);
            stream << ptr->data_ << " ";
            _InOrder(ptr->right_tree_);
        }
    }

    void destory(Node* ptr)
    {
        if (ptr)
        {
            destory(ptr->left_tree_);
            destory(ptr->right_tree_);
            free_node(ptr);
        }
    }

    // 写代码就像运输，如果你提前把背包打开，那么就要拿着打开的背包赶路了
    static Node* find(Node* ptr, const T& val)
    {
        while(ptr != nullptr && alg::neq(ptr->data_, val))
        {
            ptr = alg::le(val, ptr->data_) ? ptr->left_tree_ : ptr->right_tree_;
        }

        // if(ptr == nullptr) return ptr;

        // for (;;)
        // {
        //     if (alg::le(val, ptr->data_))
        //     {
        //         ptr = ptr->left_tree_;
        //         if (ptr == nullptr) break;
        //     }
        //     else if (alg::le(ptr->data_, val))
        //     {
        //         ptr = ptr->right_tree_;
        //         if (ptr == nullptr) break;
        //     }
        //     else
        //     {
        //         break;
        //     }
        // }

        return ptr;
    }

    static Node* _first(Node* ptr)
    {
        if (ptr)
        {
            while(ptr->left_tree_)
            {
                ptr = ptr->left_tree_;
            }
        }

        return ptr;
    }

    Node* last()
    {
        return _last(_m_impl._root);
    }

    static Node* _last(Node* ptr)
    {
        if (ptr)
        {
            while(ptr->right_tree_)
            {
                ptr = ptr->right_tree_;
            }
        }

        return ptr;
    }

    static Node* prev(Node* ptr)
    {
        if (ptr == nullptr) return ptr;

        if (ptr->left_tree_)
        {
            return _last(ptr->left_tree_);
        }
        else
        {
            Node* pa = ptr->parent_;
            while(pa != nullptr && pa->right_tree_ != ptr)
            {
                ptr = pa;
                pa = pa->parent_;
            }
            return pa;
        }
    }

};

}


#endif