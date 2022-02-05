#ifndef _TOOLS_BINARY_RB_TREE2_H_
#define _TOOLS_BINARY_RB_TREE2_H_

#include <memory>
#include <cassert>

#include "vector.hpp"
#include "queue.h"
#include "algorithm.hpp"

#include "binary_rb_tree_base.h"

#define HAS_COLOR
#include "binary_tree_print_util.h"

namespace tools
{

namespace rb_tree_2
{

template<typename T>
struct _RbNode
{
    _RbNode() = default;
    _RbNode(const T&val) : data_(val) {}
    _RbNode(const T&val, Color c) : data_(val), color_(c) {}

    _RbNode* left_tree_{nullptr};
    _RbNode* right_tree_{nullptr};
    _RbNode* parent_{nullptr};
    Color color_{Color::Black};
    T data_;
};

/*红黑树 非递归*/
template<typename T,
        typename Alloc = std::allocator<T>,
        template <typename T1> class RbNode = _RbNode>
class RbTree : protected RbTree_Base<T, RbNode, Alloc>, protected RbTree_Util2<T, RbNode>
{
    typedef RbTree_Base<T, RbNode, Alloc> RbTreeBase;
    typedef RbTree_Util2<T, RbNode> RbTreeUtil2;

public:
    typedef typename RbTreeBase::Node Node;
    typedef Node*  Root;

public:
    using RbTreeBase::buy_node;
    using RbTreeBase::free_node;
    using RbTreeBase::_m_impl;

    // 算法函数
    using RbTreeUtil2::handle_before_remove;
    using RbTreeUtil2::search_for_delete_min2;
    using RbTreeUtil2::search_for_delete_max2;

    using RbTreeUtil2::SignalRotateLeft;
    using RbTreeUtil2::SignalRotateRight;
    using RbTreeUtil2::doubleRotateLeft;
    using RbTreeUtil2::doubleRotateRight;
    using RbTreeUtil2::SignalRotateRightForRemove;
    using RbTreeUtil2::ChangeColorAfterRemoveRightRotate;
    using RbTreeUtil2::SignalRotateLeftForRemove;
    using RbTreeUtil2::ChangeColorAfterRemoveLeftRotate;
    using RbTreeUtil2::doubleRotateRightForRemove;
    using RbTreeUtil2::doubleRotateLeftForRemove;

    using RbTreeUtil2::get_color;
    using RbTreeUtil2::_get_hight;
    using RbTreeUtil2::_InOrder;

    using RbTreeUtil2::first;
    using RbTreeUtil2::next;
    using RbTreeUtil2::last;
    using RbTreeUtil2::prev;

public:
    RbTree()
    {
        initialize();
    }

    ~RbTree()
    {
        destory(_m_impl._root);
        _m_impl._root = nullptr;
    }

    RbTree(const RbTree& rhs)
    {
        initialize();
        _m_impl._root->right_tree_ = copy(rhs._m_impl._root->right_tree_);
    }

    RbTree& operator=(const RbTree& rhs)
    {
        if (this != &rhs)
        {
            if (_m_impl._root->right_tree_ != nullptr)
                destory(_m_impl._root->right_tree_);
            _m_impl._root->right_tree_ = copy(rhs._m_impl._root->right_tree_);
        }
        return *this;
    }

    RbTree(RbTree&& rhs)   noexcept
    {
        initialize();
        _m_impl._root->right_tree_ = rhs._m_impl._root->right_tree_;
        // 确保对象生存周期内任何情况下 _m_impl._root 都不空
        rhs._m_impl._root->right_tree_ = nullptr;
    }

    RbTree& operator=(RbTree&& rhs)  noexcept
    {
        if (this != &rhs)
        {
            _m_impl._root->right_tree_ = rhs._m_impl._root->right_tree_;
            // 确保对象生存周期内任何情况下 _m_impl._root 都不空
            rhs._m_impl._root->right_tree_ = nullptr;
        }
        return *this;
    }

    // 自顶向下插入 非递归
    bool insert(const T& val)
    {
        return insert(_m_impl._root, val);
    }

    // 自顶向下删除 非递归
    bool remove(const T& val)
    {
        bool res = remove(_m_impl._root, val);

        if (_m_impl._root->right_tree_)
            _m_impl._root->right_tree_->color_ = Color::Black;
        return res;
    }

    Node* find(const T& val)
    {
        Node* ptr = _m_impl._root->right_tree_;
        while(ptr != nullptr && alg::neq(ptr->data_, val))
        {
            ptr = alg::le(val, ptr->data_) ? ptr->left_tree_ : ptr->right_tree_;
        }

        return ptr;
    }

    bool is_rb_tree()
    {   
        Node* read_root = _m_impl._root->right_tree_;
        if (read_root == nullptr) return true;

        if (read_root->color_ == Color::Red) return false;

        return _is_rb_tree(read_root).first;
    }

    int32_t get_hight()
    {
        return _get_hight(_m_impl._root->right_tree_);
    }

    Node* get_root()
    {
        return _m_impl._root->right_tree_;
    }

    void in_order()
    {
        _InOrder(_m_impl._root->right_tree_);
        stream << std::endl;
    }

    // 返回元素个数
    size_t NiceInOrder() const
    {
        size_t e_size = 0;
        Node* ptr = first(_m_impl._root->right_tree_);
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

    // 返回元素个数
    size_t ResNiceInOrder() const
    {
        size_t e_size = 0;
        Node* ptr = last(_m_impl._root->right_tree_);
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

    void print_tree()
    {
        draw_tree<Node>(_m_impl._root->right_tree_);
    }

    bool is_same(const RbTree& rhs)
    {
        return check_same(_m_impl._root->right_tree_, rhs._m_impl._root->right_tree_) &&
               check_same2(_m_impl._root->right_tree_, rhs._m_impl._root->right_tree_);
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
            new_ptr->color_ = left->color_;
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
            alg::eq(ptr->color_, ptr2->color_) &&
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

    bool insert(Node* root, const T& val)
    {
        Node* ptr = root->right_tree_;
        Node* pa = root;   // 父节点
        if (ptr == nullptr)
        {
            pa->right_tree_ = buy_node(val);
            return true;
        }
        // Node* gptr = root; // 祖父节点
        // Node* _gptr = root; // 曾祖父节点

        Dir insert_dir = Dir::Unknown;
        while(ptr != nullptr)
        {
            if (get_color(ptr->left_tree_) == Color::Red && get_color(ptr->right_tree_) == Color::Red)
            {
                // 分裂 4-节点
                ptr->left_tree_->color_ = Color::Black;
                ptr->right_tree_->color_ = Color::Black;
                ptr->color_ = Color::Red;
                balance_check(ptr, val);

                // 还原根节点颜色, 只有在父节点是root的情况下才可能出现
                // pa在balance_check之后可能会被改变，不过这里仅仅使用值做判断
                if (pa == _m_impl._root)
                    ptr->color_ = Color::Black;
                    //_m_impl._root->right_tree_->color_ = Color::Black;
            }

            // 如果经过 balance_check之后, 理论上下面的值都可能会被修改
            // 该算法之所以正确，是在于: 经过了处理后，ptr会从原来gptr的位置继续向下遍历
            // 遍历的过程，一定不会出现需要再次 balance_check的场景，随着ptr的遍历
            // _gptr, gptr, pa 等指针的值也会慢慢全部被替换为正确的值，后面的逻辑也就保证了正确
            // _gptr = gptr;  gptr = pa; pa = ptr;
            pa = ptr;

            if (alg::le(val, ptr->data_))
            {
                ptr = ptr->left_tree_;
                insert_dir = Dir::Left;
            }
            else if (alg::le(ptr->data_, val))
            {
                ptr = ptr->right_tree_;
                insert_dir = Dir::Right;
            }
            else
            {
                return false;
            }
        }
        
        // ptr == nullptr
        // 默认初始插入节点颜色为 Red
        if (insert_dir == Dir::Left)
        {
            ptr = buy_node(val, Color::Red);
            pa->left_tree_ = ptr;
        }
        else
        {
            ptr = buy_node(val, Color::Red);
            pa->right_tree_ = ptr;
        }
        ptr->parent_ = pa;

        // 可能存在两个红色节点相连
        if (pa->color_ == Color::Red)
            balance_check(ptr, val);

        return true;
    }

    // check p_ptr和 p_pa 是否同时为red节点
    // 将所有的单旋转和双旋转同时进行考虑
    // 进入该函数最小的树为_gptr为_m_impl.root, gptr为实际的root, pa 和 ptr 同时为红节点
    void balance_check(Node* ptr, const T& val)
    {
        Node* pa = ptr->parent_;
        if (pa == nullptr) return;  // ptr是实际的根节点

        if (ptr->color_ == Color::Red && pa->color_ == Color::Red)
        {
            Node* gptr = pa->parent_;
            assert(gptr->color_ == Color::Black);
            // 如果曾祖父节点是根节点，那么默认是右边
            // 该判断位于ptr和pa节点为红色的情况下，在程序一开始的过程中，也就是 gptr和_gptr都是
            // 根节点， 是不可能进入到这里的。
            Node* _gptr = nullptr;
            Dir _gptr_dir = Dir::Right;
            if (gptr == _m_impl._root->right_tree_)
            {
                _gptr = _m_impl._root;
            }
            else
            {
                _gptr = gptr->parent_;
                _gptr_dir = alg::le(gptr->data_, _gptr->data_) ? Dir::Left : Dir::Right;
            }

            if (alg::le(ptr->data_, gptr->data_))
            {
                // 左旋转
                if (alg::le(ptr->data_, pa->data_)) // 单旋转
                {
                    if (_gptr_dir == Dir::Left)
                        _gptr->left_tree_ = SignalRotateLeft(gptr);
                    else
                        _gptr->right_tree_ = SignalRotateLeft(gptr);
                }
                else // 双旋转
                {
                    if (_gptr_dir == Dir::Left)
                        _gptr->left_tree_ = doubleRotateLeft(gptr);
                    else
                        _gptr->right_tree_ = doubleRotateLeft(gptr);
                }
            }
            else
            {
                // 右旋转
                if (alg::le(pa->data_, ptr->data_)) // 单旋转
                {
                    if (_gptr_dir == Dir::Left)
                        _gptr->left_tree_ = SignalRotateRight(gptr);
                    else
                        _gptr->right_tree_ = SignalRotateRight(gptr);
                }
                else  // 双旋转
                {
                    if (_gptr_dir == Dir::Left)
                        _gptr->left_tree_ = doubleRotateRight(gptr);
                    else
                        _gptr->right_tree_ = doubleRotateRight(gptr);
                }
            }
        }
    }

    bool remove(Node* ptr, const T& val_param)
    {
        Node* pptr = ptr;      // 作为遍历的父节点
        ptr = ptr->right_tree_;
        T val = val_param;       // 后面可能会修改待删除的val值
        handle_before_remove(ptr, pptr, val);

        Dir dir = Dir::Right;
        Dir dir_change = Dir::Unknown; // 控制替换节点后的方向变化
        bool res = false;
        Node* old_ptr = nullptr; // 代替ptr记录状态
        while(ptr != nullptr)
        {
            if (alg::le(val, ptr->data_))
            {
                res = search_for_delete_min2(ptr, val, false);
                dir == Dir::Left ? pptr->left_tree_ = ptr : pptr->right_tree_ = ptr;
                if (res)
                {
                    pptr = ptr->left_tree_;
                    ptr = pptr->left_tree_;
                }
                else
                {
                    pptr = ptr;
                    ptr = ptr->left_tree_;
                }
                dir = Dir::Left;
            }
            else if (alg::le(ptr->data_, val))
            {
                res = search_for_delete_max2(ptr, val, false);   // 仅依据数据来转向
                dir == Dir::Left ? pptr->left_tree_ = ptr : pptr->right_tree_ = ptr;
                if (res)
                {
                    pptr = ptr->right_tree_;  // 更新成ptr最新的父指针
                    ptr = pptr->right_tree_;
                }
                else
                {
                    pptr = ptr;
                    ptr = ptr->right_tree_;
                }
                dir = Dir::Right;
            }
            else
            {
                old_ptr = ptr;             // 重置
                dir_change = Dir::Unknown; // 重置
                if ((ptr->left_tree_ != nullptr && ptr->right_tree_ != nullptr) ||
                    ptr->right_tree_ != nullptr)
                {
                    // 这里处理之后，会发生遍历线路的拐弯，需要特殊处理
                    res = search_for_delete_max2(ptr, val, true, true); // 强制转向
                    dir == Dir::Left ? pptr->left_tree_ = ptr : pptr->right_tree_ = ptr;
                    if (res)
                    {
                        ptr = ptr->right_tree_->right_tree_;
                    }
                    else
                    {
                        ptr = ptr->right_tree_;
                    }

                    pptr = ptr->parent_;
                    while(ptr->left_tree_)
                    {
                        res = search_for_delete_min2(ptr, val, true);  // 去掉数据和外界的强制转向
                        if (res)
                        {
                            if (dir_change == Dir::Unknown)
                            {
                                pptr->right_tree_ = ptr;
                                dir_change = Dir::Left;
                            }
                            else
                            {
                                pptr->left_tree_ = ptr;
                            }
                            ptr = ptr->left_tree_->left_tree_;
                        }
                        else
                        {
                            if (dir_change == Dir::Unknown)
                                dir_change = Dir::Left;
                            ptr = ptr->left_tree_;
                        }
                        pptr = ptr->parent_;
                    }
                    old_ptr->data_ = ptr->data_;
                    val = ptr->data_;
                    dir_change == Dir::Unknown ? dir = Dir::Right : dir = Dir::Left;
                }
                else if (ptr->left_tree_ != nullptr)
                {
                    res = search_for_delete_min2(ptr, val, true, true);
                    dir == Dir::Left ? pptr->left_tree_ = ptr : pptr->right_tree_ = ptr;
                    if (res)
                    {
                        ptr = ptr->left_tree_->left_tree_;
                    }
                    else
                    {
                        ptr = ptr->left_tree_;
                    }

                    pptr = ptr->parent_;
                    while(ptr->right_tree_) // 当右子节点存在时，再进行接下来的处理
                    {
                        res = search_for_delete_max2(ptr, val, true);
                        if (res)
                        {
                            if (dir_change == Dir::Unknown)
                            {
                                pptr->left_tree_ = ptr;
                                dir_change = Dir::Right;
                            }
                            else
                            {
                                pptr->right_tree_ = ptr; // 更新变更的ptr指针的父节点
                            }
                            ptr = ptr->right_tree_->right_tree_;
                        }
                        else
                        {
                            if (dir_change == Dir::Unknown)
                                dir_change = Dir::Right;
                            ptr = ptr->right_tree_;
                        }
                        pptr = ptr->parent_;
                    }
                    old_ptr->data_ = ptr->data_;
                    val = ptr->data_;
                    dir_change == Dir::Unknown ? dir = Dir::Left : dir = Dir::Right;
                }
                else
                {
                    if (ptr != _m_impl._root->right_tree_)
                        assert(ptr->color_ == Color::Red);
                    free_node(ptr);
                    ptr = nullptr;
                    dir == Dir::Left ? pptr->left_tree_ = ptr : pptr->right_tree_ = ptr;
                    return true;
                }
            }
        }

        // 没有找到，返回false
        return false;
    }

    using RbInfo = std::pair<bool/*is rb_tree ?*/, uint32_t/*size of black_node*/>;
    RbInfo _is_rb_tree(Node* ptr)
    {
        if (ptr == nullptr) return RbInfo(true, 0);

        RbInfo res(false, 0);
        Color c = ptr->color_;

        do
        {
            if (c == Color::Red)
            {
                if ((ptr->right_tree_ && ptr->right_tree_->color_ == c) ||
                    (ptr->left_tree_ && ptr->left_tree_->color_ == c))
                {
                    break;
                }
            }

            RbInfo l_info = _is_rb_tree(ptr->left_tree_);
            if (!l_info.first) break;
            RbInfo r_info = _is_rb_tree(ptr->right_tree_);
            if (!r_info.first) break;

            if (l_info.second != r_info.second) break;

            bool is_sort = true;
            if (ptr->left_tree_)
                is_sort &= alg::le(ptr->left_tree_->data_, ptr->data_);
            if (ptr->right_tree_)
                is_sort &= alg::le(ptr->data_, ptr->right_tree_->data_);

            res.first = is_sort;
            res.second = l_info.second + (c == Color::Black ? 1 : 0);
        } while (false);

        // if (ptr->data_ == T(140))
        // {
        //     draw_tree<Node>(ptr);
        // }
        // if (!res.first)
        // {
            // draw_tree<Node>(ptr);
            // assert(false);
        // }

        return res;
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

    void initialize()
    {
        if (null_node_ == nullptr)
            null_node_ = buy_node(T());

        if (_m_impl._root == nullptr)
        {
            _m_impl._root = buy_node(T());
            _m_impl._root->left_tree_ = null_node_;
        }
    }


private:
    // TODO static 需要处理生命周期线程安全
    /*static*/ Node* null_node_{nullptr};
};

// template<typename T, typename Alloc, template <typename T1> class RbNode>
// typename RbTree<T, Alloc, RbNode>::Node*  RbTree<T, Alloc, RbNode>::null_node_;

}
}

#endif