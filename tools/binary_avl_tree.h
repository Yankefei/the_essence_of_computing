#ifndef _TOOLS_BINARY_AVL_TREE_H_
#define _TOOLS_BINARY_AVL_TREE_H_

#include <memory>
#include <cassert>

#include "vector.hpp"
#include "queue.h"
#include "algorithm.hpp"
#include "binary_avl_tree_util.h"

namespace tools
{

template<typename T>
struct _BNode
{
    _BNode() = default;
    _BNode(const T&val) : data_(val) {}

    _BNode* left_tree_{nullptr};
    _BNode* right_tree_{nullptr};
    int32_t hight_{-1};    // 树高
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

    // 重复元素插入失败
    bool insert(const T& val)
    {
        return insert(_m_impl._root, val);
    }

    bool insert2(const T& val)
    {
        return insert2(&_m_impl._root, val);
    }

    bool remove(const T& val)
    {
        bool res = remove(_m_impl._root, val, nullptr);
        if (res)
        {
            reset_hight(_m_impl._root);
        }

        return res;
    }

    bool remove2(const T& val)
    {
        return remove2(&_m_impl._root, val);
    }

    void InOrder()
    {
        _InOrder(_m_impl._root);
        stream << std::endl;
    }

    void LevelOrder()
    {
        _LevelOrder(_m_impl._root);
    }

    Root get_root()
    {
        return _m_impl._root;
    }

    int32_t get_hight()
    {
        return _get_hight(_m_impl._root);
    }

    bool is_balance()
    {
        //return is_balance2(_m_impl._root);
        return is_balance(_m_impl._root).second;
    }

    void print_tree()
    {
        draw_tree<Node>(_m_impl._root);
    }

private:
    // 递归
    bool insert(Node*& ptr, const T& val)
    {
        if (ptr == nullptr)
        {
            ptr = buy_node(val);
            ptr->hight_ = 0;
            return true;
        }

        if (alg::gt(ptr->data_, val))
        {
            if (!insert(ptr->left_tree_, val)) return false;

            if (hight(ptr->left_tree_) - hight(ptr->right_tree_) == 2)
            {
                // 调整之后，必须保证当前的的子树内部高度已经保持正常
                // 1. 保证平衡， 2. 节点高度是正确的
                if (alg::gt(ptr->left_tree_->data_, val))
                {
                    // 这里传递二级指针可以，因为修改函数内部*ptr的信息，就相当于修改外部ptr的值
                    // 而该值为引用传参
                    signal_rotate_withleft(&ptr);
                }
                else
                {
                    // 如果把这里和下面双旋转的地方都注释掉，最后的插入结果还是平衡的
                    // 是因为即使没有直接进行双旋转，但是后面还会使用对应的单旋转逻辑进行矫正
                    double_rotate_withleft(&ptr);
                }
            }
        }
        else if (alg::le(ptr->data_, val))
        {
            if (!insert(ptr->right_tree_, val)) return false;

            if (hight(ptr->right_tree_) - hight(ptr->left_tree_) == 2)
            {
                if (alg::le(ptr->right_tree_->data_, val))
                {
                    signal_rotate_withright(&ptr);
                }
                else
                {
                    double_rotate_withright(&ptr);
                }
            }
        }
        else
        {
            return false;
        }

        // 这里计算的结果是当前节点的高度值
        ptr->hight_ = alg::max(hight(ptr->left_tree_), hight(ptr->right_tree_)) + 1;

        return true;
    }

    bool remove(Node*& ptr, const T& val, Node* f_ptr)
    {
        if (ptr == nullptr) return false;

        if (alg::gt(ptr->data_, val))
        {
            return remove(ptr->left_tree_, val, ptr);
        }
        else if (alg::le(ptr->data_, val))
        {
            return remove(ptr->right_tree_, val, ptr);
        }
        else
        {
            Node* check = nullptr;  // 被删除的节点的对称节点
            Node* tmp = nullptr;    // 待删除的节点
            bool need_bal = false;  // 如不需要平衡，则节点自身的高度数据也无需更改
            if (ptr->left_tree_ && ptr->right_tree_)
            {
                tmp = find_max(ptr->left_tree_);
                ptr->data_ = tmp->data_;

                Node* tmp2 = ptr->left_tree_;
                if (tmp2->right_tree_ == nullptr)
                {
                    if (hight(ptr->right_tree_) - hight(tmp2) == 1)
                    {
                        need_bal = true;
                        check = ptr->right_tree_;
                    }

                    f_ptr = ptr;
                }
                else
                {
                    while (tmp2->right_tree_)
                    {
                        if (tmp2->right_tree_ == tmp)
                        {
                            f_ptr = tmp2;
                            check = f_ptr->left_tree_;
                        }
                        tmp2 = tmp2->right_tree_;
                    }

                    assert(f_ptr != nullptr);
                    if (hight(f_ptr->left_tree_) - hight(tmp->right_tree_) == 1)
                    {
                        need_bal = true;
                    }
                }
                f_ptr->right_tree_ = tmp->left_tree_;
            }
            else
            {
                check = f_ptr->left_tree_ == ptr ?
                                f_ptr->right_tree_ : f_ptr->left_tree_;
                if (hight(check) - hight(ptr) == 1)
                {
                    need_bal = true;
                }

                Node* tmp = ptr;

                if (ptr->left_tree_ == nullptr)
                {
                    ptr = ptr->right_tree_;
                }
                else if (ptr->right_tree_ == nullptr) // 判断可省略
                {
                    ptr = ptr->left_tree_;
                }
            }

            if (need_bal && check != nullptr && !(check->right_tree_ == nullptr && check->left_tree_ == nullptr))
            {
                // 再次获取双亲节点
                Node* p = parent(_m_impl._root, f_ptr);
                if (alg::gt(check->data_, val))
                {
                    if (check->right_tree_ == nullptr && check->left_tree_)
                    {
                        double_rotate_withright(&f_ptr);
                    }
                    else
                    {
                        signal_rotate_withright(&f_ptr);
                    }
                    p->right_tree_ = f_ptr;
                }
                else
                {
                    if (check->right_tree_ && check->left_tree_ == nullptr)
                    {
                        double_rotate_withleft(&f_ptr);
                    }
                    else
                    {
                        signal_rotate_withleft(&f_ptr);
                    }
                    p->left_tree_ = f_ptr;
                }
            }

            free_node(tmp);

            return true;
        }
    }

    //  非递归
    bool insert2(Node** pptr, const T& val)
    {
        if (pptr == nullptr) false;

        Node* ptr = *pptr;
        while(ptr != nullptr && alg::neq(ptr->data_, val))
        {
            ptr = alg::gt(ptr->data_, val) ? ptr->left_tree_ : ptr->right_tree_;
        }
    }

    bool remove2(Node** ptr, const T& val)
    {

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

    static void signal_rotate_withleft(Node** pptr)
    {
        Node* k2 = *pptr;
        Node* k1 = k2->left_tree_;
        k2->left_tree_ = k1->right_tree_;
        k1->right_tree_ = k2;

        // 处理高度
        k2->hight_ = alg::max(hight(k2->left_tree_), hight(k2->right_tree_)) + 1;
        k1->hight_ = alg::max(hight(k1->left_tree_), k2->hight_) + 1;

        *pptr = k1;
    }

    static void signal_rotate_withright(Node** pptr)
    {
        Node* k1 = *pptr;
        Node* k2 = k1->right_tree_;
        k1->right_tree_ = k2->left_tree_;
        k2->left_tree_ = k1;

        k1->hight_ = alg::max(hight(k1->left_tree_), hight(k1->right_tree_)) + 1;
        k2->hight_ = alg::max(hight(k2->right_tree_), k1->hight_) + 1;

        *pptr = k2;
    }

    static void double_rotate_withleft(Node** pptr)
    {
        signal_rotate_withright(&(*pptr)->left_tree_);
        signal_rotate_withleft(pptr);
    }

    static void double_rotate_withright(Node** pptr)
    {
        signal_rotate_withleft(&(*pptr)->right_tree_);
        signal_rotate_withright(pptr);
    }

    static uint32_t reset_hight(Node* ptr)
    {
        if (ptr == nullptr) return -1;

        ptr->hight_ = alg::max(reset_hight(ptr->left_tree_), reset_hight(ptr->right_tree_)) + 1;

        return ptr->hight_;
    }

    static int32_t _get_hight(Node* ptr)
    {
        if (ptr == nullptr) return -1;

        return alg::max(_get_hight(ptr->left_tree_), _get_hight(ptr->right_tree_)) + 1;
    }

    // 判断是否子树是否平衡的函数
    static std::pair<int32_t, bool> is_balance(Node* ptr)
    {
        if (ptr == nullptr) return {-1, true};

        std::pair<int32_t, bool> l_res = {-1, false};
        l_res = is_balance(ptr->left_tree_);

        std::pair<int32_t, bool> r_res = {-1, false};
        r_res = is_balance(ptr->right_tree_);

        int32_t diff = l_res.first > r_res.first ?
                        (l_res.first - r_res.first) :
                        (r_res.first - l_res.first);

        return {alg::max(l_res.first, r_res.first) + 1,
                (diff < 2) && l_res.second && r_res.second};
    }

    static bool is_balance2(Node* ptr)
    {
        if (ptr == nullptr) return true;

        int32_t left_h = hight(ptr->left_tree_);
        int32_t right_h = hight(ptr->right_tree_);

        if (left_h != right_h)
        {
            int32_t diff = left_h > right_h ? (left_h - right_h) : (right_h - left_h);
            if (diff >= 2) return false;
        }

        return is_balance2(ptr->left_tree_) && is_balance2(ptr->right_tree_);
    }

    static Node* find_max(Node* ptr)
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

    static Node* find_min(Node* ptr)
    {

    }

    static int32_t hight(Node* ptr)
    {
        if (ptr == nullptr)
            return -1;
        else
            return ptr->hight_;
    }

    void _InOrder(Node* ptr)
    {
        if (ptr)
        {
            _InOrder(ptr->left_tree_);
            stream << ptr->data_ << " ";
            _InOrder(ptr->right_tree_);
        }
    }

    void _LevelOrder(Node* ptr)
    {

    }

    Node* parent(Node* ptr, Node* child)
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
};

}

#endif