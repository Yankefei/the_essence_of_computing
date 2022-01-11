#ifndef _TOOLS_BINARY_AVL_TREE2_H_
#define _TOOLS_BINARY_AVL_TREE2_H_

#include <memory>
#include <cassert>

#include "stack.h"
#include "algorithm.hpp"

#define HAS_BALANCE

#include "binary_avl_tree_util.h"

namespace tools
{

/*平衡状态*/
enum class Bal
{
    Right = -1,
    Balance = 0,
    Left = 1
};

template<typename T>
struct _BNode
{
    _BNode() = default;
    _BNode(const T&val) : data_(val) {}

    _BNode* left_tree_{nullptr};
    _BNode* right_tree_{nullptr};
    Bal balance_{Bal::Balance};    // 平衡因子 1 : left   0 : balance  -1 : right
    T data_;
};


template<typename T, typename Alloc>
class AvlTree_Base
{
public:
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<_BNode<T>> _BNode_alloc_type;

    typedef typename std::allocator_traits<_BNode_alloc_type>
        rebind_traits;

    typedef typename rebind_traits::pointer  pointer;

    typedef _BNode<T>        Node;

public:
    struct Base_Impl : public _BNode_alloc_type
    {
        Base_Impl(): _BNode_alloc_type(), _root()
        {
        }

        ~Base_Impl() {}

        Base_Impl(Base_Impl& _x) noexcept
        {
            std::swap(_root, _x._root);
        }

        Node*     _root{nullptr};
    };

    AvlTree_Base() {}
    ~AvlTree_Base() {} // 设置虚函数，Base_Impl 会额外增加8字节内存

public:
    Base_Impl   _m_impl;

    Node* buy_node(const T& val)
    {
        pointer p = rebind_traits::allocate(_m_impl, 1);
        rebind_traits::construct(_m_impl, p, val);
        return static_cast<Node*>(p);
    }

    void free_node(Node* ptr)
    {
        rebind_traits::destroy(_m_impl, ptr);
        rebind_traits::deallocate(_m_impl, ptr, 1);
    }
};

/*排序平衡二叉树*/
template<typename T, typename Alloc = std::allocator<T>>
class AvlTree : protected AvlTree_Base<T, Alloc>
{
    typedef typename AvlTree_Base<T, Alloc>::Node Node;
    typedef Node*  Root;

public:
    using AvlTree_Base<T, Alloc>::buy_node;
    using AvlTree_Base<T, Alloc>::free_node;
    using AvlTree_Base<T, Alloc>::_m_impl;

public:
    AvlTree() {}
    ~AvlTree() { destory(_m_impl._root); }

    // 递归
    bool insert2(const T& val) // pass
    {
        bool taller = false;
        return insert(_m_impl._root, val, taller);
    }
    // 递归
    bool remove2(const T& val)   // pass
    {
        bool lower = false;
        return remove(_m_impl._root, val, lower);
    }

    // 非递归
    bool insert(const T& val) // error
    {
        return insert(&_m_impl._root, val);
    }
    // 非递归
    bool remove(const T& val)  // pass
    {
        return remove(&_m_impl._root, val);
    }

    void InOrder()
    {
        _InOrder(_m_impl._root);
        stream << std::endl;
    }

    Node* get_root() const
    {
        return _m_impl._root;
    }

    bool is_balance() const
    {
        return _is_balance(_m_impl._root).second;
    }

    void print_tree() const
    {
        draw_tree<Node>(_m_impl._root);
    }

    int32_t get_hight() const
    {
        return _get_hight(_m_impl._root);
    }

private:
    // 递归
    bool insert(Node*& ptr, const T&val, bool& taller)
    {
        if (ptr == nullptr)
        {
            ptr = buy_node(val);
            taller = true;
            return true;
        }

        if (alg::gt(ptr->data_, val))
        {
            if (!insert(ptr->left_tree_, val, taller)) return false;
            if (taller)
            {
                switch(ptr->balance_)
                {
                    case Bal::Balance:
                    {
                        ptr->balance_ = Bal::Left; // 增加节点的传递链不能断
                        break;
                    }
                    case Bal::Left:
                    {
                        ptr = total_left_balance(ptr);
                        taller = false; // 重新恢复平衡或者本该的平衡状态时，阻断增加节点的传递链
                        break;
                    }
                    case Bal::Right:
                    {
                        ptr->balance_ = Bal::Balance;
                        taller = false;
                        break;
                    }
                    default:
                        break;
                }
            }
            return true;
        }
        else if(alg::le(ptr->data_, val))
        {
            if (!insert(ptr->right_tree_, val ,taller)) return false;
            if (taller)
            {
                switch(ptr->balance_)
                {
                    case Bal::Balance:
                    {
                        ptr->balance_ = Bal::Right;
                        break;
                    }
                    case Bal::Left:
                    {
                        ptr->balance_ = Bal::Balance;
                        taller = false;
                        break;
                    }
                    case Bal::Right:
                    {
                        ptr = total_right_balance(ptr);
                        taller = false;
                        break;
                    }
                    default:
                        break;
                }
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    // 递归
    bool remove(Node*& ptr, const T&val, bool& lower)
    {
        if (ptr == nullptr)
            return false;
        
        if (alg::gt(ptr->data_, val))
        {
            if (!remove(ptr->left_tree_, val, lower)) return false;
            if (lower)
            {
                change_left_tree_after_remove(ptr, lower);
            }
            return true;
        }
        else if (alg::le(ptr->data_, val))
        {
            if (!remove(ptr->right_tree_, val, lower)) return false;
            if (lower)
            {
                change_right_tree_after_remove(ptr, lower);
            }
            return true;
        }
        else
        {
            if (ptr->left_tree_ && ptr->right_tree_)
            {
                Node* q = ptr->left_tree_; // 寻找左子树的最大值替换
                while (q->right_tree_)
                {
                    q = q->right_tree_;
                }
                ptr->data_ = q->data_;
                remove(ptr->left_tree_, ptr->data_, lower); // 继续递归
                if (lower)
                {
                    change_left_tree_after_remove(ptr, lower);
                }
            }
            else
            {
                Node* child = ptr->left_tree_ == nullptr ? ptr->right_tree_ : ptr->left_tree_;
                free_node(ptr);
                ptr = child;
                lower = true;
            }
            return true;
        }
    }

    // 记录遍历过程父节点的指针和遍历方向
    struct ParentNode
    {
        ParentNode(Node* _ptr, Bal _dir) : ptr(_ptr), dir(_dir) {}
        Node* ptr;
        Bal   dir{Bal::Balance};
    };

    // 非递归
    bool insert(Node** pptr, const T& val)
    {
        if (pptr == nullptr) return false;

        Node* ptr = *pptr;
        Stack<ParentNode> st;
        while(ptr != nullptr && alg::neq(ptr->data_, val))
        {
            if (alg::gt(ptr->data_, val))
            {
                st.push(ParentNode(ptr, Bal::Left));
                ptr = ptr->left_tree_;
            }
            else
            {
                st.push(ParentNode(ptr, Bal::Right));
                ptr = ptr->right_tree_;
            }
        }

        if (ptr != nullptr) return false;

        ptr = buy_node(val);
        if (st.empty())
        {
            *pptr = ptr; return true;
        }
        else
        {
            ParentNode node = st.top();
            st.pop();
            if (node.dir == Bal::Left)
                node.ptr->left_tree_ = ptr;
            else
                node.ptr->right_tree_ = ptr;

            switch(node.ptr->balance_)
            {
                case Bal::Balance:
                {
                    if (node.dir == Bal::Left)
                        node.ptr->balance_ = Bal::Left;
                    else
                        node.ptr->balance_ = Bal::Right;
                    break;
                }
                case Bal::Left:
                case Bal::Right:
                {
                    node.ptr->balance_ = Bal::Balance;
                    break;
                }
                default:
                    break;
            }

            // 已经达到平衡条件，可以退出
            if (node.ptr->balance_ == Bal::Balance)
            {
                return true;
            }
        }

        Node* ready_ptr = nullptr; // 放置一个待修改的指针
        while(!st.empty())
        {
            ParentNode node = st.top();
            st.pop();

            if (ready_ptr)
            {
                if (node.dir == Bal::Left)
                    node.ptr->left_tree_ = ready_ptr;
                else
                    node.ptr->right_tree_ = ready_ptr;
                
                ready_ptr = nullptr;
                break;       // 停止转播
            }
            switch(node.ptr->balance_)
            {
                case Bal::Balance:
                {
                    if (node.dir == Bal::Left)
                        node.ptr->balance_ = Bal::Left;
                    else
                        node.ptr->balance_ = Bal::Right;
                    break;
                }
                case Bal::Left:
                {
                    if (node.dir == Bal::Right)
                    {
                        node.ptr->balance_ = Bal::Balance;
                        return true;
                    }
                    else
                        ready_ptr = total_left_balance(node.ptr);
                    break;
                }
                case Bal::Right:
                {
                    if (node.dir == Bal::Left)
                    {
                        node.ptr->balance_ = Bal::Balance;
                        return true;
                    }
                    else
                        ready_ptr = total_right_balance(node.ptr);
                    break;
                }
                default:
                    break;
            }
        }

        if (ready_ptr)
            *pptr = ready_ptr;

        return true;
    }

    // 非递归
    bool remove(Node** pptr, const T&val)
    {
        if (pptr == nullptr) return false;

        Node* ptr = *pptr;
        Stack<ParentNode> st;
        while(ptr != nullptr && alg::neq(ptr->data_, val))
        {
            if (alg::gt(ptr->data_, val))
            {
                st.push(ParentNode(ptr, Bal::Left));
                ptr = ptr->left_tree_;
            }
            else
            {
                st.push(ParentNode(ptr, Bal::Right));
                ptr = ptr->right_tree_;
            }
        }

        if (ptr == nullptr) return false;
        if (ptr->left_tree_ && ptr->right_tree_)
        {
            st.push(ParentNode(ptr, Bal::Left));
            Node* q = ptr->left_tree_; // 将右子树最大值转移
            while(q->right_tree_)
            {
                st.push(ParentNode(q, Bal::Right));
                q = q->right_tree_;
            }
            ptr->data_ = q->data_;
            ptr = q;    // 后面删除ptr即可
        }

        Node* child = ptr->left_tree_ == nullptr ? ptr->right_tree_ : ptr->left_tree_;
        free_node(ptr);

        Node* ready_ptr = nullptr;
        bool lower = true;  // 开始状态为存在删除的节点
        if(st.empty())
        {
            *pptr = child;
        }
        else
        {
            ParentNode node = st.top();
            st.pop();
            if (node.dir == Bal::Left)
            {
                node.ptr->left_tree_ = child;
                change_left_tree_after_remove(node.ptr, lower);
                ready_ptr = node.ptr;
            }
            else
            {
                node.ptr->right_tree_ = child;
                change_right_tree_after_remove(node.ptr, lower);
                ready_ptr = node.ptr;
            }
        }

        while(!st.empty())
        {
            ParentNode node = st.top();
            st.pop();

            if (node.dir == Bal::Left)
                node.ptr->left_tree_ = ready_ptr;
            else
                node.ptr->right_tree_ = ready_ptr;

            if (!lower)
            {
                ready_ptr = nullptr;
                break;  // 结束循环，跳出
            }

            if (node.dir == Bal::Left)
            {
                change_left_tree_after_remove(node.ptr, lower);
                ready_ptr = node.ptr;
            }
            else
            {
                change_right_tree_after_remove(node.ptr, lower);
                ready_ptr = node.ptr;
            }
        }

        if (ready_ptr)
            *pptr = ready_ptr;  // 最后必须给pptr赋值一次

        return true;
    }

    // 删除左子树后的调整
    void change_left_tree_after_remove(Node*& ptr, bool& lower)
    {
        switch(ptr->balance_)
        {
            case Bal::Balance:
            {
                ptr->balance_ = Bal::Right;
                lower = false; // 停止向上传递
                break;
            }
            case Bal::Right:
            {
                if (lower)
                {
                    Bal r_b = ptr->right_tree_->balance_;
                    switch(r_b)
                    {
                        case Bal::Balance:
                        {
                            ptr->balance_ = Bal::Right;
                            ptr->right_tree_->balance_ = Bal::Left;
                            right_balance(ptr);
                            lower = false;  // 这种情况停止向上传递，因为旋转后子树高度不降低
                            break;
                        }
                        case Bal::Left:
                        {
                            double_right_bal_change(ptr);
                            double_right_balance(ptr);
                            break;
                        }
                        case Bal::Right:
                        {
                            ptr->balance_ = Bal::Balance;
                            ptr->right_tree_->balance_ = Bal::Balance; // 恢复平衡
                            right_balance(ptr);
                            break;
                        }
                        default:
                            break;
                    }
                }
                // lower 需要继续传递
                break;
            }
            case Bal::Left:
            {
                 // 从平衡到非平衡，表示层数减少, 这个变化还是需要传递到上层
                ptr->balance_ = Bal::Balance;
                break;
            }
            default:
                break;
        }
    }

    // 删除右子树后的调整
    void change_right_tree_after_remove(Node*& ptr, bool& lower)
    {
        switch(ptr->balance_)
        {
            case Bal::Balance:
            {
                ptr->balance_ = Bal::Left;
                lower = false; // 停止向上传递
                break;
            }
            case Bal::Left:
            {
                if (lower)
                {
                    Bal r_b = ptr->left_tree_->balance_;
                    switch(r_b)
                    {
                        // 需要特殊处理当前左子树的节点
                        case Bal::Balance:
                        {
                            ptr->balance_ = Bal::Left;
                            ptr->left_tree_->balance_ = Bal::Right;
                            left_balance(ptr);
                            lower = false;      // 这种情况停止向上传递，因为旋转后子树高度不降低
                            break;
                        }
                        case Bal::Left:
                        {
                            ptr->balance_ = Bal::Balance;
                            ptr->left_tree_->balance_ = Bal::Balance; // 恢复平衡
                            left_balance(ptr);
                            break;
                        }
                        case Bal::Right:
                        {
                            double_left_bal_change(ptr);
                            double_left_balance(ptr);
                            break;
                        }
                        default:
                            break;
                    }
                }
                // lower 需要继续传递
                break;
            }
            case Bal::Right:
            {
                ptr->balance_ = Bal::Balance;
                // lower 需要继续传递
                break;
            }
            default:
                break;
        }
    }

    // 这里的平衡因子状态变化，请查阅状态变化图：
    // 清华大学，严蔚敏版的《数据结构（C语言版）》P235页，将所有的情况都进行了归纳
    // 返回新的顶点
    Node* total_left_balance(Node* ptr)
    {
        if (ptr->left_tree_->balance_ == Bal::Left)
        {
            ptr->balance_ = Bal::Balance;
            ptr->left_tree_->balance_ = Bal::Balance;
            left_balance(ptr);
        }
        else
        {
            double_left_bal_change(ptr);
            double_left_balance(ptr);
        }

        return ptr;
    }
    // 返回新的顶点
    Node* total_right_balance(Node* ptr)
    {
        if (ptr->right_tree_->balance_ == Bal::Right)
        {
            ptr->balance_ = Bal::Balance;
            ptr->right_tree_->balance_ = Bal::Balance;
            right_balance(ptr);
        }
        else
        {
            double_right_bal_change(ptr);
            double_right_balance(ptr);
        }

        return ptr;
    }

    void double_left_bal_change(Node* ptr)
    {
        Node* b = ptr->left_tree_;
        Node* c = b->right_tree_;
        switch(c->balance_)
        {
            case Bal::Balance:
            {
                ptr->balance_ = Bal::Balance;
                b->balance_ = Bal::Balance;
                break;
            }
            case Bal::Left:
            {
                ptr->balance_ = Bal::Right;
                b->balance_ = Bal::Balance;
                break;
            }
            case Bal::Right:
            {
                ptr->balance_ = Bal::Balance;
                b->balance_ = Bal::Left;
                break;
            }
            default:
                break;
        }
        c->balance_ = Bal::Balance;
    }

    Bal double_right_bal_change(Node* ptr)
    {
        Node* b = ptr->right_tree_;
        Node* c = b->left_tree_;
        switch(c->balance_)
        {
            case Bal::Balance:
            {
                ptr->balance_ = Bal::Balance;
                b->balance_ = Bal::Balance;
                break;
            }
            case Bal::Left:
            {
                ptr->balance_ = Bal::Balance;
                b->balance_ = Bal::Right;
                break;
            }
            case Bal::Right:
            {
                ptr->balance_ = Bal::Left;
                b->balance_ = Bal::Balance;
                break;
            }
            default:
                break;
        }
        c->balance_ = Bal::Balance;
    }

    static void left_balance(Node*& ptr)
    {
        Node* b = ptr->left_tree_;
        ptr->left_tree_ = b->right_tree_;
        b->right_tree_ = ptr;
        ptr = b;
    }

    static void right_balance(Node*& ptr)
    {
        Node* b = ptr->right_tree_;
        ptr->right_tree_ = b->left_tree_;
        b->left_tree_ = ptr;
        ptr = b;
    }

    static void double_left_balance(Node*& ptr)
    {
        right_balance(ptr->left_tree_);
        left_balance(ptr);
    }

    static void double_right_balance(Node*& ptr)
    {
        left_balance(ptr->right_tree_);
        right_balance(ptr);
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

    static int32_t _get_hight(Node* ptr)
    {
        if (ptr == nullptr) return -1;

        return alg::max(_get_hight(ptr->left_tree_), _get_hight(ptr->right_tree_)) + 1;
    }

    using BalRes = std::pair<int32_t, bool>;
    static BalRes _is_balance(Node* ptr)
    {
        if (ptr == nullptr) return BalRes(-1, true);

        BalRes left_res = _is_balance(ptr->left_tree_);
        BalRes right_res = _is_balance(ptr->right_tree_);

        int32_t res = left_res.first > right_res.first ?
                    (left_res.first - right_res.first) :
                    (right_res.first - left_res.first);

        // if (res >= 2)
        // {
        //     draw_tree<Node>(ptr);
        //     assert(false);
        // }

        return BalRes(alg::max(left_res.first, right_res.first) + 1,
                            (res < 2) && left_res.second && right_res.second);
    }
};

}

#endif