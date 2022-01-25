#ifndef _TOOLS_BINARY_AVL_TREE3_H_
#define _TOOLS_BINARY_AVL_TREE3_H_

#include <memory>
#include <cassert>

#include "algorithm.hpp"
#include "stack.h"

#include "binary_avl_tree_base.h"

#define HAS_BALANCE
#include "binary_tree_print_util.h"

namespace tools
{

namespace avl_tree_3
{

template<typename T>
struct _BNode
{
    _BNode() = default;
    _BNode(const T&val) : data_(val) {}

    _BNode* left_tree_{nullptr};
    _BNode* right_tree_{nullptr};
    _BNode* parent_{nullptr};      // 父指针是迭代器遍历树的关键
    Bal balance_{Bal::Balance};    // 平衡因子 1 : left   0 : balance  -1 right
    T data_;
};



// 当默认模板参数本身也是模板时, 需要使用 template<typename T1> class 的形式来替换 typename
/*排序平衡二叉树*/
template<typename T, typename Alloc = std::allocator<T> ,template <typename T1> class BNode = _BNode>
class AvlTree : protected AvlTree_Base<T, BNode, Alloc>
{
    typedef AvlTree_Base<T, BNode, Alloc> AvlTreeBase;
public:
    typedef typename AvlTreeBase::Node Node;
    typedef Node*  Root;

public:
    using AvlTreeBase::buy_node;
    using AvlTreeBase::free_node;
    using AvlTreeBase::_m_impl;

public:
    AvlTree() {}
    ~AvlTree() { destory(_m_impl._root); }

    AvlTree(const AvlTree& rhs)
    {
        _m_impl._root = copy(rhs._m_impl._root);
    }

    AvlTree& operator=(const AvlTree& rhs)
    {
        if (this != &rhs)
        {
            destory(_m_impl._root);
            _m_impl._root = copy(rhs._m_impl._root);
        }
        return *this;
    }

    AvlTree(AvlTree&& rhs)   noexcept
    {
        _m_impl._root = rhs._m_impl._root;
        rhs._m_impl._root = nullptr;
    }

    AvlTree& operator=(AvlTree&& rhs)   noexcept
    {
        if (this != &rhs)
        {
            _m_impl._root = rhs._m_impl._root;
            rhs._m_impl._root = nullptr;
        }
        return *this;
    }

    bool insert2(const T& val)  // pass
    {
        bool taller = false;
        return insert(_m_impl._root, val, nullptr, taller);
    }

    bool remove2(const T& val)   // pass
    {
        bool lower = false;
        return remove(_m_impl._root, val, lower);
    }

    // 非递归
    bool insert(const T& val)   // pass
    {
        return insert(&_m_impl._root, val);
    }

    // 非递归
    bool remove(const T& val)     // pass
    {
        return remove(&_m_impl._root, val);
    }

    void InOrder() const
    {
        _InOrder(_m_impl._root);
        stream << std::endl;
    }

    size_t NiceInOrder() const
    {
        size_t e_size = 0;
        Node* ptr = first(_m_impl._root);
        T f_min = ptr->data_;
        for (; ptr != nullptr; ptr = next(ptr))
        {
            if (alg::le(ptr->data_, f_min))
            {
                assert(false);
            }
            f_min = ptr->data_;
            // stream << ptr->data_ << " ";
            e_size ++;
        }
        // stream << std::endl;
        return e_size;
    }

    size_t ResNiceInOrder() const
    {
        size_t e_size = 0;
        Node* ptr = last(_m_impl._root);
        T f_max = ptr->data_;
        for (; ptr != nullptr; ptr = prev(ptr))
        {
            if (alg::le(f_max, ptr->data_))
            {
                assert(false);
            }
            f_max = ptr->data_;
            // stream << ptr->data_ << " ";
            e_size ++;
        }
        // stream << std::endl;
        return e_size;
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

    bool is_same(const AvlTree& rhs)
    {
        return check_same(_m_impl._root, rhs._m_impl._root) &&
               check_same2(_m_impl._root, rhs._m_impl._root);
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
            new_ptr->balance_ = left->balance_;
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
            alg::eq(ptr->balance_, ptr2->balance_) &&
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
            Node* ptr = first(root1);
            Node* ptr2 = first(root2);
            for (; ptr != nullptr; ptr = next(ptr), ptr2 = next(ptr2))
            {
                if (alg::neq(ptr->data_, ptr2->data_))
                    return false;
            }
            if (ptr2 != nullptr) return false;
        }

        {
            Node* ptr = last(root1);
            Node* ptr2 = last(root2);
            for (; ptr != nullptr; ptr = prev(ptr), ptr2 = prev(ptr2))
            {
                if (alg::neq(ptr->data_, ptr2->data_))
                    return false;
            }
            if (ptr2 != nullptr) return false;
        }
        return true;
    }

    // 这里的taller是在递归中必须存在的，告知上层是否应该停止平衡判定的操作
    bool insert(Node*& ptr, const T&val, Node* f_ptr, bool& taller)
    {
        if (ptr == nullptr)
        {
            ptr = buy_node(val);
            ptr->parent_ = f_ptr;
            taller = true;
            return true;
        }

        if (alg::le(val, ptr->data_))
        {
            if (!insert(ptr->left_tree_, val, ptr, taller)) return false;
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
            if (!insert(ptr->right_tree_, val, ptr, taller)) return false;
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

    bool remove(Node*& ptr, const T& val, bool& lower)
    {
        if (ptr == nullptr)
            return false;
        
        if (alg::le(val, ptr->data_))
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
                if (child)
                    child->parent_ = ptr->parent_;
                free_node(ptr);
                ptr = child;
                lower = true;
            }
            return true;
        }
    }

    // 非递归
    bool insert(Node** pptr, const T& val)
    {
        if (pptr == nullptr) return false;

        Node *pa = nullptr;
        Node* ptr = *pptr;
        while(ptr != nullptr && alg::neq(ptr->data_, val))
        {
            if (alg::le(val, ptr->data_))
            {
                pa = ptr; ptr = ptr->left_tree_;
            }
            else
            {
                pa = ptr; ptr = ptr->right_tree_;
            }
        }

        if (ptr != nullptr) return false;

        ptr = buy_node(val);
        ptr->parent_ = pa;
        if (pa == nullptr)
        {
            *pptr = ptr; return true;
        }
        else
        {
            Bal dir = alg::le(val, pa->data_) ? Bal::Left : Bal::Right;
            if (dir == Bal::Left)
                pa->left_tree_ = ptr;
            else
                pa->right_tree_ = ptr;

            switch(pa->balance_)
            {
                case Bal::Balance:
                {
                    if (dir == Bal::Left)
                        pa->balance_ = Bal::Left;
                    else
                        pa->balance_ = Bal::Right;
                    break;
                }
                case Bal::Left:
                case Bal::Right:
                {
                    pa->balance_ = Bal::Balance;
                    break;
                }
                default:
                    break;
            }

            // 已经达到平衡条件，可以退出
            if (pa->balance_ == Bal::Balance)
            {
                return true;
            }
        }

        Node* ready_ptr = nullptr; // 放置一个待修改的指针
        while(pa->parent_ != nullptr)
        {
            Node* pa_ = pa->parent_;
            Bal dir = pa_->left_tree_ == pa ? Bal::Left : Bal::Right;
            pa = pa_;

            switch(pa_->balance_)
            {
                case Bal::Balance:
                {
                    if (dir == Bal::Left)
                        pa_->balance_ = Bal::Left;
                    else
                        pa_->balance_ = Bal::Right;
                    break;
                }
                case Bal::Left:
                {
                    if (dir == Bal::Right)
                    {
                        pa_->balance_ = Bal::Balance;
                        return true;
                    }
                    else
                        ready_ptr = total_left_balance(pa_);
                    break;
                }
                case Bal::Right:
                {
                    if (dir == Bal::Left)
                    {
                        pa_->balance_ = Bal::Balance;
                        return true;
                    }
                    else
                        ready_ptr = total_right_balance(pa_);
                    break;
                }
                default:
                    break;
            }

            if (ready_ptr)
            {
                // ready_ptr->parent_ == nullptr 表示ready_ptr已经是根节点了
                if (ready_ptr->parent_ == nullptr)
                    break;

                // 设置ready_ptr的父节点如何指向自身
                Bal _dir = alg::le(val, ready_ptr->parent_->data_) ? Bal::Left : Bal::Right;

                if (_dir == Bal::Left)
                    ready_ptr->parent_->left_tree_ = ready_ptr;
                else
                    ready_ptr->parent_->right_tree_ = ready_ptr;
                ready_ptr = nullptr;
                // 或者可以修改 pa的指针为ready_ptr, 因此此时pa, pa_都可能被修改了状态
                break;       // 停止转播
            }
        }

        if (ready_ptr)  // 更新根节点
            *pptr = ready_ptr;

        return true;
    }

    // 非递归
    bool remove(Node** pptr, const T&val)
    {
        if (pptr == nullptr) return false;

        T t_val = val;
        Node* ptr = *pptr;
        while(ptr != nullptr && alg::neq(ptr->data_, t_val))
        {
            if (alg::le(t_val, ptr->data_))
                ptr = ptr->left_tree_;
            else
                ptr = ptr->right_tree_;
        }

        if (ptr == nullptr) return false;

        if (ptr->left_tree_ && ptr->right_tree_)
        {
            Node* q = ptr->left_tree_; // 将右子树最大值转移
            while(q->right_tree_)
                q = q->right_tree_;
            ptr->data_ = q->data_;
            ptr = q;    // 后面删除ptr即可
            t_val = ptr->data_;
        }

        Node* pa = ptr->parent_;
        Node* child = ptr->left_tree_ == nullptr ? ptr->right_tree_ : ptr->left_tree_;
        if (child)
            child->parent_ = ptr->parent_;

        free_node(ptr);

        Node* ready_ptr = nullptr;
        bool lower = true;  // 开始状态为存在删除的节点
        bool is_rotate = false; // 是否旋转
        Node* tmp_pa = pa;
        if (tmp_pa == nullptr)
        {
            *pptr = child;
            return true;
        }
        else
        {
            Bal dir = tmp_pa->left_tree_ == ptr ? Bal::Left : Bal::Right;
            if (dir == Bal::Left)
            {
                tmp_pa->left_tree_ = child;
                is_rotate = change_left_tree_after_remove(tmp_pa, lower);
                ready_ptr = tmp_pa;
            }
            else
            {
                tmp_pa->right_tree_ = child;
                is_rotate = change_right_tree_after_remove(tmp_pa, lower);
                ready_ptr = tmp_pa;
            }

            if (is_rotate)
            {
                if (set_status_after_remove(ready_ptr, t_val, pptr))
                {
                    if (ready_ptr)  *pptr = ready_ptr;  // 最后必须给pptr赋值一次
                    return true;
                }

                pa = ready_ptr; // 修改pa的指针
                is_rotate = false;
            }
        }

        while(pa->parent_)
        {
            if (!lower)
            {
                ready_ptr = nullptr;
                break;  // 结束循环，跳出
            }

            tmp_pa = pa->parent_;
            Bal dir = tmp_pa->left_tree_ == pa ? Bal::Left : Bal::Right;
            pa = tmp_pa;

            if (dir == Bal::Left)
            {
                is_rotate = change_left_tree_after_remove(tmp_pa, lower);
                ready_ptr = tmp_pa;
            }
            else
            {
                is_rotate = change_right_tree_after_remove(tmp_pa, lower);
                ready_ptr = tmp_pa;
            }

            if (is_rotate)
            {
                if (set_status_after_remove(ready_ptr, t_val, pptr))
                    break;
                pa = ready_ptr; // 修改pa的指针
                is_rotate = false;
            }
        }

        if (ready_ptr)  *pptr = ready_ptr;  // 最后必须给pptr赋值一次

        return true;
    }

    bool set_status_after_remove(Node* ready_ptr, const T& val, Node** pptr)
    {
        // ready_ptr->parent_ == nullptr 表示ready_ptr已经是根节点了
        if (ready_ptr->parent_ == nullptr)
        {
            return true;
        }

        // 设置ready_ptr的父节点如何指向自身
        Bal _dir = !alg::le(ready_ptr->parent_->data_, val) ? Bal::Left : Bal::Right;

        if (_dir == Bal::Left)
            ready_ptr->parent_->left_tree_ = ready_ptr;
        else
            ready_ptr->parent_->right_tree_ = ready_ptr;
        
        return false;
    }

    // 删除左子树后的调整
    bool change_left_tree_after_remove(Node*& ptr, bool& lower)
    {
        bool is_rotate = false;
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
                    is_rotate = true;
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

        return is_rotate;
    }

    // 删除右子树后的调整
    bool change_right_tree_after_remove(Node*& ptr, bool& lower)
    {
        bool is_rotate = false;
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
                    is_rotate = true;
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

        return is_rotate;
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

    /*  注： 目前所有的转换函数，已经将内部所有节点的状态信息全部变更完毕
        外界使用则无需任何更改，且注意内部的状态也会同时发生改变
    */
    static void left_balance(Node*& ptr)
    {
        Node* b = ptr->left_tree_;
        ptr->left_tree_ = b->right_tree_;
        b->right_tree_ = ptr;

        // 处理parent_指针的指向
        b->parent_ = ptr->parent_;
        ptr->parent_ = b;
        if (ptr->left_tree_)
            ptr->left_tree_->parent_ = ptr;

        ptr = b;
    }

    static void right_balance(Node*& ptr)
    {
        Node* b = ptr->right_tree_;
        ptr->right_tree_ = b->left_tree_;
        b->left_tree_ = ptr;

        // 处理parent_指针的指向
        b->parent_ = ptr->parent_;
        ptr->parent_ = b;
        if (ptr->right_tree_)
            ptr->right_tree_->parent_ = ptr;

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
            if (ptr->left_tree_) _InOrder(ptr->left_tree_);
            stream << ptr->data_ << " ";
            if (ptr->right_tree_) _InOrder(ptr->right_tree_);
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

        bool res = left_res.first > right_res.first ?
                    left_res.first - right_res.first < 2 :
                    right_res.first - left_res.first < 2;
        
        bool is_sort = true;
        if (ptr->left_tree_)
            is_sort &= alg::le(ptr->left_tree_->data_, ptr->data_);
        if (ptr->right_tree_)
            is_sort &= alg::le(ptr->data_, ptr->right_tree_->data_);

        return BalRes(alg::max(left_res.first, right_res.first) + 1,
                            is_sort && res && left_res.second && right_res.second);
    }

    static Node* first(Node* ptr)
    {
        if (ptr)
        {
            while (ptr->left_tree_)
                ptr = ptr->left_tree_;
        }

        return ptr;
    }

    static Node* next(Node* ptr)
    {
        if (ptr)
        {
            if (ptr->right_tree_)
                return first(ptr->right_tree_);
            else
            {
                Node* pa = ptr->parent_;
                while(pa != nullptr && pa->right_tree_ == ptr)
                {
                    ptr = pa;
                    pa = pa->parent_;
                }
                return pa;
            }
        }
    }

    static Node* last(Node* ptr)
    {
        if (ptr)
        {
            while(ptr->right_tree_)
                ptr = ptr->right_tree_;
        }
        return ptr;
    }

    static Node* prev(Node* ptr)
    {
        if (ptr)
        {
            if (ptr->left_tree_)
                return last(ptr->left_tree_);
            else
            {
                Node* pa = ptr->parent_;
                while(pa != nullptr && pa->left_tree_ == ptr)
                {
                    ptr = pa;
                    pa = pa->parent_;
                }
                return pa;
            }
        }
    }
};

}
}

#endif