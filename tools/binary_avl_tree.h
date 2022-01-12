#ifndef _TOOLS_BINARY_AVL_TREE_H_
#define _TOOLS_BINARY_AVL_TREE_H_

#include <memory>
#include <cassert>

#include "vector.hpp"
#include "queue.h"
#include "algorithm.hpp"
#include "binary_tree_print_util.h"

namespace tools
{

namespace avl_tree_1
{

enum class Dir
{
    Unknown,
    Left,
    Right
};

template<typename T>
struct _BNode
{
    _BNode() = default;
    _BNode(const T&val) : data_(val), hight_(0) {}

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

      // 非递归
    bool insert(const T& val)          // pass
    {
        return insert2(&_m_impl._root, val);
    }

    // 非递归
    bool remove(const T& val)          // pass
    {
        return remove2(&_m_impl._root, val);
    }

    // 递归
    bool insert2(const T& val)         // pass
    {
        return insert(_m_impl._root, val);     
    }

    // 递归
    bool remove2(const T& val)                
    {
        // 正确版本的递归函数
        return _remove(_m_impl._root, val);   // pass
        //return remove(_m_impl._root, val, nullptr);  // error
    }

    void InOrder()
    {
        _InOrder(_m_impl._root);
        stream << std::endl;
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

    // 递归
    // 事实证明，如果算法没有想明白，直接写，代码总是很扯淡, 会有无情无尽的bug等待着你
    // 该算法逻辑异常，放弃调试
    bool remove(Node*& ptr, const T& val, Node* f_ptr)
    {
        if (ptr == nullptr) return false;

        if (alg::gt(ptr->data_, val))
        {
            if (!remove(ptr->left_tree_, val, ptr))
                return false;
            
            if (!right_balance_check(&ptr))
            {
                // 重新计算
                ptr->hight_ = alg::max(hight(ptr->right_tree_), hight(ptr->left_tree_)) + 1;
            }

            return true;
        }
        else if (alg::le(ptr->data_, val))
        {
            if (!remove(ptr->right_tree_, val, ptr))
                return false;

            if (!left_balance_check(&ptr))
            {
                // 重新计算
                ptr->hight_ = alg::max(hight(ptr->right_tree_), hight(ptr->left_tree_)) + 1;
            }

            return true;
        }
        else
        {
            Node* old_ptr = ptr;
            Node* tmp = nullptr;    // 待删除的节点
            bool no_need_bal = true;  // 是否要无需平衡, true 表示无需平衡，false表示不一定

            if (ptr->left_tree_ && ptr->right_tree_)
            {
                Node* q = ptr;
                tmp = ptr->left_tree_;
                //Node* old_tmp = tmp; // 仅仅用于更新树高
                //Stack<Node*> right_st; // 用栈来保存遍历路径。。。
                while(tmp->right_tree_)
                {
                    //right_st.push(tmp);
                    q = tmp;
                    tmp = tmp->right_tree_;
                }
                ptr->data_ = tmp->data_;

                // 下面的f_ptr为真正删除的节点的父节点，非ptr的父节点
                if (q == ptr)
                {
                    f_ptr = q;
                    q->left_tree_ = tmp->left_tree_;
                    no_need_bal = q->right_tree_ == nullptr;
                }
                else
                {
                    f_ptr = q;
                    q->right_tree_ = tmp->left_tree_;
                    // 这里的问题在于，还需要将q到ptr整个链路上所有的节点高度重新计算一遍
                    // 而且还必须是**边调整平衡边计算**，因为递归的线路没有包含这里
                    // 所以该算法为错误的算法，不能仅仅使用堆栈来弥补
                    // 错误的点在于，如果将ptr的双子节点非空的情况和其他情况一起处理，情况会非常复杂
                    // while(right_st.size() != 1)
                    // {
                    //     Node* p = right_st.top();
                    //     right_st.pop();
                    //     p->hight_ = alg::max(hight(p->right_tree_), hight(p->left_tree_)) + 1;
                    // }

                    no_need_bal = q->left_tree_ == nullptr;
                }

                // 当ptr为根节点，需要额外处理
                if (ptr == _m_impl._root)
                {
                    // 单独更新高度变化, 删除节点后，如果节点高度相同，则需要手动更新
                    if (!no_need_bal && hight(f_ptr->left_tree_) == hight(f_ptr->right_tree_))
                    {
                        f_ptr->hight_ = f_ptr->left_tree_->hight_ + 1;
                    }
                    right_balance_check(&ptr);
                }
            }
            else
            {
                if (f_ptr != nullptr)
                {
                    no_need_bal = f_ptr->left_tree_ == ptr ?
                                (f_ptr->right_tree_ == nullptr) : (f_ptr->left_tree_ == nullptr);
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

            do
            {
                if (no_need_bal)
                    break;

                Node* check = f_ptr;
                if (left_balance_check(&f_ptr)) // 如果返回true, f_ptr指针会被修改
                {
                    // 需要再次获取双亲节点
                    Node* other_f_ptr = parent(_m_impl._root, check);

                    // f_ptr 可能变为root节点
                    if (other_f_ptr != nullptr)
                    {
                        if (other_f_ptr == old_ptr)  // 双亲指针和待删除的ptr一致
                        {
                            other_f_ptr->left_tree_ = f_ptr;
                        }else if(other_f_ptr->left_tree_ == check)
                            other_f_ptr->left_tree_ = f_ptr;
                        else
                            other_f_ptr->right_tree_ = f_ptr;
                    }
                    else
                        _m_impl._root = f_ptr;
                }
                else if (right_balance_check(&f_ptr))
                {
                    // 需要再次获取双亲节点
                    Node* other_f_ptr = parent(_m_impl._root, check);

                    if (other_f_ptr != nullptr)
                    {
                        if (other_f_ptr == old_ptr)
                        {
                            other_f_ptr->right_tree_ = f_ptr; // 应该不会发生
                            assert(false);
                        }else if (other_f_ptr->right_tree_ == check)
                            other_f_ptr->right_tree_ = f_ptr;
                        else
                            other_f_ptr->left_tree_ = f_ptr;
                    }
                    else
                        _m_impl._root = f_ptr;
                }
            }while(false);
            
            free_node(tmp);

            return true;
        }
    }

    // 递归的另外一个版本, 正确的版本，逻辑都是简单清晰的
    bool _remove(Node*& ptr, const T& val)
    {
        if (ptr == nullptr) return false;

        if (alg::gt(ptr->data_, val))
        {
            if (!_remove(ptr->left_tree_, val))
                return false;
            
            if (!right_balance_check(&ptr))
            {
                // 重新计算
                ptr->hight_ = alg::max(hight(ptr->right_tree_), hight(ptr->left_tree_)) + 1;
            }
        }
        else if (alg::le(ptr->data_, val))
        {
            if (!_remove(ptr->right_tree_, val))
                return false;

            if (!left_balance_check(&ptr))
            {
                // 重新计算
                ptr->hight_ = alg::max(hight(ptr->right_tree_), hight(ptr->left_tree_)) + 1;
            }
        }
        else
        {
            if (ptr->left_tree_ && ptr->right_tree_)
            {
                Node* q = ptr->left_tree_; // 取右子树的最大值
                while(q->right_tree_)
                {
                    q = q->right_tree_;
                }
                ptr->data_ = q->data_;
                _remove(ptr->left_tree_, q->data_);

                if (!right_balance_check(&ptr))
                {
                    // 重新计算
                    ptr->hight_ = alg::max(hight(ptr->right_tree_), hight(ptr->left_tree_)) + 1;
                }
            }
            else
            {
                Node* child = ptr->left_tree_ == nullptr ? ptr->right_tree_ : ptr->left_tree_;
                free_node(ptr);
                ptr = child;
            }
        }
        return true;
    }

    // 记录遍历过程父节点的指针和遍历方向
    struct ParentNode
    {
        ParentNode(Node* _ptr, Dir _dir) : ptr(_ptr), dir(_dir) {}
        Node* ptr;
        Dir   dir{Dir::Unknown};
    };

    //  非递归
    bool insert2(Node** pptr, const T& val)
    {
        if (pptr == nullptr) return false;

        Node* ptr = *pptr;
        Stack<ParentNode> st;
        while(ptr != nullptr && alg::neq(ptr->data_, val))
        {
            if (alg::gt(ptr->data_, val))
            {
                st.push(ParentNode(ptr, Dir::Left));
                ptr = ptr->left_tree_;
            }
            else
            {
                st.push(ParentNode(ptr, Dir::Right));
                ptr = ptr->right_tree_;
            }
        }

        if (ptr != nullptr) return false;

        ptr = buy_node(val);
        if (st.empty())
        {
            *pptr = ptr;
            return true;
        }
        else
        {
            ParentNode node = st.top();
            st.pop();
            if (node.dir == Dir::Left)
                node.ptr->left_tree_ = ptr;
            else
                node.ptr->right_tree_ = ptr;

            node.ptr->hight_ = alg::max(hight(node.ptr->left_tree_), hight(node.ptr->right_tree_)) + 1;
        }

        Node* ready_ptr = nullptr; // 放置一个待修改的指针，只要非nullptr, 说明需要修改节点指针
        while(!st.empty())
        {
            ParentNode node = st.top();
            st.pop();
            if (node.dir == Dir::Left)
            {
                if (ready_ptr)
                {
                    node.ptr->left_tree_ = ready_ptr;
                    ready_ptr = nullptr;
                }
                if (hight(node.ptr->left_tree_) - hight(node.ptr->right_tree_) == 2)
                {
                    if (alg::gt(node.ptr->left_tree_->data_, val))
                    {
                        signal_rotate_withleft(&node.ptr);
                        ready_ptr = node.ptr;
                    }
                    else
                    {
                        double_rotate_withleft(&node.ptr);
                        ready_ptr = node.ptr;
                    }
                }
            }
            else
            {
                if (ready_ptr)
                {
                    node.ptr->right_tree_ = ready_ptr;
                    ready_ptr = nullptr;
                }
                if (hight(node.ptr->right_tree_) - hight(node.ptr->left_tree_) == 2)
                {
                    if (alg::le(node.ptr->right_tree_->data_, val))
                    {
                        signal_rotate_withright(&node.ptr);
                        ready_ptr = node.ptr;
                    }
                    else
                    {
                        double_rotate_withright(&node.ptr);
                        ready_ptr = node.ptr;
                    }
                }
            }

            node.ptr->hight_ = alg::max(hight(node.ptr->left_tree_), hight(node.ptr->right_tree_)) + 1;
        }

        if (ready_ptr)
            *pptr = ready_ptr;

        return true;
    }

    // 非递归
    bool remove2(Node** pptr, const T& val)
    {
        if (pptr == nullptr) false;

        Node* ptr = *pptr;
        Stack<ParentNode> st;
        while(ptr != nullptr && alg::neq(ptr->data_, val))
        {
            if (alg::gt(ptr->data_, val))
            {
                st.push(ParentNode(ptr, Dir::Left));
                ptr = ptr->left_tree_;
            }
            else
            {
                st.push(ParentNode(ptr, Dir::Right));
                ptr = ptr->right_tree_;
            }
        }
        if (ptr == nullptr) return false;
        if (ptr->left_tree_ && ptr->right_tree_)
        {
            st.push(ParentNode(ptr, Dir::Left));
            Node* q = ptr->left_tree_; // 将右子树最大值转移过来
            while(q->right_tree_)
            {
                st.push(ParentNode(q, Dir::Right));
                q = q->right_tree_;
            }
            ptr->data_ = q->data_;
            ptr = q; // 只需要删除ptr就可以，可以和下面的情况一起处理          
        }

        Node* child = ptr->left_tree_ == nullptr ? ptr->right_tree_ : ptr->left_tree_;
        Node* ready_ptr = nullptr;
        if(st.empty())
        {
            *pptr = child;
            return true;
        }
        else
        {
            ParentNode node = st.top();
            st.pop();
            if (node.dir == Dir::Left)
            {
                node.ptr->left_tree_ = child;
                if (right_balance_check(&node.ptr))
                {
                    ready_ptr = node.ptr;
                }
            }
            else
            {
                node.ptr->right_tree_ = child;
                if (left_balance_check(&node.ptr))
                {
                    ready_ptr = node.ptr;
                }
            }

            node.ptr->hight_ = alg::max(hight(node.ptr->left_tree_), hight(node.ptr->right_tree_)) + 1;
        }

        while(!st.empty())
        {
            ParentNode node = st.top();
            st.pop();
            if (node.dir == Dir::Left)
            {
                if (ready_ptr)
                {
                    node.ptr->left_tree_ = ready_ptr;
                    ready_ptr = nullptr;
                }
                if (right_balance_check(&node.ptr))
                {
                    ready_ptr = node.ptr;
                }
            }
            else
            {
                if (ready_ptr)
                {
                    node.ptr->right_tree_ = ready_ptr;
                    ready_ptr = nullptr;
                }
                if (left_balance_check(&node.ptr))
                {
                    ready_ptr = node.ptr;
                }
            }

            node.ptr->hight_ = alg::max(hight(node.ptr->left_tree_), hight(node.ptr->right_tree_)) + 1;
        }

        if (ready_ptr)
            *pptr = ready_ptr;

        return true;
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

    bool right_balance_check(Node** pptr)
    {
        Node* ptr = *pptr;
        // 需要重新进行平衡
        if (hight(ptr->right_tree_) - hight(ptr->left_tree_) == 2)
        {
            Node* r_ptr = ptr->right_tree_;
            if (hight(r_ptr->left_tree_) > hight(r_ptr->right_tree_))
            {
                double_rotate_withright(pptr);
            }
            else
            {
                signal_rotate_withright(pptr);
            }
            return true;
        }
        return false;
    }

    bool left_balance_check(Node** pptr)
    {
        Node* ptr = *pptr;
        // 需要重新进行平衡
        if (hight(ptr->left_tree_) - hight(ptr->right_tree_) == 2)
        {
            Node* l_ptr = ptr->left_tree_;
            if (hight(l_ptr->right_tree_) > hight(l_ptr->left_tree_))
            {
                double_rotate_withleft(pptr);
            }
            else
            {
                signal_rotate_withleft(pptr);
            }
            return true;
        }
        return false;
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

        if (diff >= 2)
        {
            draw_tree<Node>(ptr);
            assert(false);
        }

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

    Node* parent(Node* ptr, Node* child)
    {
        if (nullptr == ptr) return ptr;
        if (ptr == child) return nullptr;

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
}

#endif