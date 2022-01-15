#ifndef _TOOLS_BINARY_RB_TREE_H_
#define _TOOLS_BINARY_RB_TREE_H_

#include <memory>
#include <cassert>

#include "vector.hpp"
#include "queue.h"
#include "algorithm.hpp"
#include "pair.hpp"

#include "binary_rb_tree_base.h"

#define HAS_COLOR
#include "binary_tree_print_util.h"

namespace tools
{

namespace rb_tree_1
{

enum class Dir
{
    Unknown = 0,
    Left,
    Right
};

template<typename T>
struct _RbNode
{
    _RbNode() = default;
    _RbNode(const T&val) : data_(val) {}
    _RbNode(const T&val, Color c) : data_(val), color_(c) {}

    _RbNode* left_tree_{nullptr};
    _RbNode* right_tree_{nullptr};
    Color color_{Color::Black};
    T data_;
};


template<typename T, typename Alloc>
class RbTree_Base
{
public:
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<_RbNode<T>> _RbNode_alloc_type;

    typedef typename std::allocator_traits<_RbNode_alloc_type>
        rebind_traits;

    typedef typename rebind_traits::pointer  pointer;

    typedef _RbNode<T>        Node;

public:
    struct Base_Impl : public _RbNode_alloc_type
    {
        Base_Impl(): _RbNode_alloc_type(), _root()
        {
        }

        ~Base_Impl() {}

        Base_Impl(Base_Impl& _x) noexcept
        {
            std::swap(_root, _x._root);
        }

        Node*     _root{nullptr};
    };

    RbTree_Base() {}
    ~RbTree_Base() {} // 设置虚函数，Base_Impl 会额外增加8字节内存

public:
    Base_Impl   _m_impl;

    Node* buy_node(const T& val)
    {
        pointer p = rebind_traits::allocate(_m_impl, 1);
        rebind_traits::construct(_m_impl, p, val);
        return static_cast<Node*>(p);
    }

    Node* buy_node(const T& val, Color c)
    {
        Node* ptr = buy_node(val);
        ptr->color_ = c;
        return ptr;
    }

    void free_node(Node* ptr)
    {
        rebind_traits::destroy(_m_impl, ptr);
        rebind_traits::deallocate(_m_impl, ptr, 1);
    }
};

/*红黑树*/
template<typename T, typename Alloc = std::allocator<T>>
class RbTree : protected RbTree_Base<T, Alloc>
{
    typedef typename RbTree_Base<T, Alloc>::Node Node;
    typedef Node*  Root;

public:
    using RbTree_Base<T, Alloc>::buy_node;
    using RbTree_Base<T, Alloc>::free_node;
    using RbTree_Base<T, Alloc>::_m_impl;

public:
    RbTree()
    {
        initialize();
    }

    ~RbTree()
    {
        if (_m_impl._root->right_tree_ != null_node_)
            destory(_m_impl._root->right_tree_);
        free_node(_m_impl._root);   
    }

    // 自顶向下插入 非递归
    bool _insert(const T& val)
    {
        return insert(_m_impl._root, val);
    }

    // 自顶向下插入 递归
    bool insert(const T&val)
    {
        Node* res = insert3(_m_impl._root->right_tree_, val);
        if (res == nullptr)
            return false;

        res->color_ = Color::Black;
        _m_impl._root->right_tree_ = res;
        return true;
    }

    // 自顶向下删除 非递归
    bool remove2(const T& val)
    {
        return remove(_m_impl._root, val);
    }

    // 自顶向下删除 递归
    // 发现了一个神奇的现象，任何删除操作，即使没有找到目标元素，也会导致整个rb_tree的染色发生变化
    // 但依然能保持rb_tree的特性
    bool remove(const T& val)
    {
        // 先进行变换， 在结束后，将根节点的颜色变为黑色即可
        bool res = remove2(_m_impl._root->right_tree_, val);
        
        if (_m_impl._root->right_tree_)
            _m_impl._root->right_tree_->color_ = Color::Black;
        return res;
    }

    bool is_rb_tree()
    {   
        Node* read_root = _m_impl._root->right_tree_;
        if (read_root == nullptr) return true;

        if (read_root->color_ == Color::Red) return false;

        return _is_rb_tree(read_root).first;
    }

    void in_order()
    {
        _InOrder(_m_impl._root->right_tree_);
    }

    void print_tree()
    {
        draw_tree<Node>(_m_impl._root->right_tree_);
    }

private:
    // 自顶向下 非递归
    // 将向下的分解和向上的旋转放在了一起处理, 和下面的递归程序截然不同
    bool insert(Node* root, const T& val)
    {
        Node* ptr = root->right_tree_;
        Node* pa = root;   // 父节点
        if (ptr == nullptr)
        {
            pa->right_tree_ = buy_node(val);
            return true;
        }
        Node* gptr = root; // 祖父节点
        Node* _gptr = root; // 曾祖父节点

        Dir insert_dir = Dir::Unknown;
        while(ptr != nullptr)
        {
            if (get_color(ptr->left_tree_) == Color::Red && get_color(ptr->right_tree_) == Color::Red)
            {
                // 这里是保证每一个节点到Null指针的路径上黑色节点个数相同的保证
                ptr->left_tree_->color_ = Color::Black;
                ptr->right_tree_->color_ = Color::Black;
                ptr->color_ = Color::Red;
                balance_check(ptr, pa, gptr, _gptr, val);
                
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
            _gptr = gptr;  gptr = pa; pa = ptr;

            if (alg::gt(ptr->data_, val))
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

        // 可能存在两个红色节点相连
        if (pa->color_ == Color::Red)
            balance_check(ptr, pa, gptr, _gptr, val);

        return true;
    }

    // 自顶向下，非递归
    bool remove(Node* ptr, const T& val)
    {

    }

    // 自顶向下 递归
    // 开始向下递归时, 分裂4-节点
    // 返回向上递归时, 判断节点连接, 进行旋转
    // 本函数在退出递归时将单旋转和双旋转分开考虑
    Node* insert3(Node* ptr, const T&val)
    {
        if (ptr == nullptr)
        {
            ptr = buy_node(val, Color::Red);
            return ptr;
        }

        if (get_color(ptr->left_tree_) == Color::Red &&
            get_color(ptr->right_tree_) == Color::Red)
        {
            ptr->color_ = Color::Red;
            ptr->left_tree_->color_ = Color::Black;
            ptr->right_tree_->color_ = Color::Black;
        }

        Node* res = nullptr;
        do
        {
            if (alg::gt(ptr->data_, val))
            {
                res = insert3(ptr->left_tree_, val);
                if (res == nullptr) break;

                // 先将下面返回的指针接到现有的ptr上
                if (res != ptr->left_tree_)
                    ptr->left_tree_ = res;

                /*关键:  判断是否应该单旋转或者双旋转，ptr必须在顶部位置*/
                if (res->color_ == Color::Red)
                {
                    if (res->left_tree_ && res->left_tree_->color_ == Color::Red)
                    {
                        // 单循环
                        res = SignalRotateLeft(ptr);
                    }
                    else if (res->right_tree_ && res->right_tree_->color_ == Color::Red)
                    {
                        // 双循环
                        res = doubleRotateLeft(ptr);
                    }
                }

                // 如果ptr没有发生改变, 那么应该照常返回
                if (ptr->left_tree_ == res)
                {
                    res = ptr; // 正常返回 ptr
                }
            }
            else if (alg::le(ptr->data_, val))
            {
                res = insert3(ptr->right_tree_, val);
                if (res == nullptr) break;

                if (res != ptr->right_tree_)
                    ptr->right_tree_ = res;

                // 这里的res就相当于ptr 的新下级节点
                if (res->color_ == Color::Red)
                {
                    if (res->left_tree_ && res->left_tree_->color_ == Color::Red)
                    {
                        res = doubleRotateRight(ptr);
                    }
                    else if (res->right_tree_ && res->right_tree_->color_ == Color::Red)
                    {
                        res = SignalRotateRight(ptr);
                    }
                }

                if (ptr->right_tree_ == res)
                {
                    res = ptr; // 正常返回 ptr
                }
            }
        }while(false);

        return res;
    }

    // 自顶向下，递归
    // 本质是: 以删除最小节点为目的的搜索函数
    bool remove2(Node*& ptr, const T& val)
    {
        if (ptr == nullptr) return false;

        if (alg::gt(ptr->data_, val))
        {
            // 根据是否旋转，判断下一个值应该如何传递，同时保证引用形参的正确性
            if (search_for_delete_min(ptr))
                return remove2(ptr->left_tree_->left_tree_, val);
            else
                return remove2(ptr->left_tree_, val);
        }
        else if (alg::le(ptr->data_, val))
        {
            if (search_for_delete_max(ptr))
                return remove2(ptr->right_tree_->right_tree_, val);
            else
                return remove2(ptr->right_tree_, val);
        }
        else
        {
            Node* pa = ptr; // 设置pa是ptr的父指针，用来连接发生变更的子节点
            Node* _ptr = nullptr; // 代替ptr记录状态， ptr无法被复用
            Node* old_ptr = ptr;
            Dir dir_change = Dir::Unknown; // 控制当前节点与父指针的方向
            if ((ptr->left_tree_ != nullptr && ptr->right_tree_ != nullptr) ||
                ptr->right_tree_ != nullptr)
            {
                if (search_for_delete_max(ptr))
                {
                    pa = ptr->right_tree_;
                    _ptr = ptr->right_tree_->right_tree_;
                }
                else
                    _ptr = ptr->right_tree_;
                while(_ptr->left_tree_)
                {
                    if (search_for_delete_min(_ptr))
                    {
                        if (dir_change == Dir::Unknown)
                        {
                            pa->right_tree_ = _ptr;
                            dir_change = Dir::Left;
                        }
                        else
                            pa->left_tree_ = _ptr;
                        pa = _ptr->left_tree_;  // 更新父指针
                        _ptr = _ptr->left_tree_->left_tree_;
                    }
                    else
                    {
                        if (dir_change == Dir::Unknown)
                            dir_change = Dir::Left;
                        pa = _ptr;
                        _ptr = _ptr->left_tree_;
                    }
                }
                old_ptr->data_ = _ptr->data_;

                // 将删除目标设置为ptr节点
                if (!remove2(_ptr, _ptr->data_)) return false;
                else
                {
                    if (dir_change == Dir::Unknown)
                        pa->right_tree_ = _ptr;
                    else
                        pa->left_tree_ = _ptr; // 最后还是更新父节点的左子树，左子树为更新后的值
                }
                return true;
            }
            else if (ptr->left_tree_ != nullptr)
            {
                if (search_for_delete_min(ptr)) // 这里的ptr如果修改了，可以通过引用让上层知道
                {
                    pa = ptr->left_tree_;  // 更新成ptr最新的父指针
                    _ptr = ptr->left_tree_->left_tree_;
                }
                else
                {
                    _ptr = ptr->left_tree_;
                }

                while(_ptr->right_tree_) // 当右子节点存在时，再进行接下来的处理
                {
                    if (search_for_delete_max(_ptr))
                    {
                        if (dir_change == Dir::Unknown)
                        {
                            pa->left_tree_ = _ptr;
                            dir_change = Dir::Right;
                        }
                        else
                            pa->right_tree_ = _ptr; // 更新变更的ptr指针的父节点
                        pa = _ptr->right_tree_;
                        _ptr = _ptr->right_tree_->right_tree_;
                    }
                    else
                    {
                        if (dir_change == Dir::Unknown)
                            dir_change = Dir::Right;
                        pa = _ptr;
                        _ptr = _ptr->right_tree_;
                    }
                }
                old_ptr->data_ = _ptr->data_;
                // 将删除目标设置为ptr节点
                if (!remove2(_ptr, _ptr->data_)) return false;
                else
                {
                     // 最后还是更新父节点的左子树，左子树为更新后的值
                    if (dir_change == Dir::Unknown)
                        pa->left_tree_ = _ptr;
                    else
                        pa->right_tree_ = _ptr;
                }
                return true;
            }
            else
            {
                if (ptr != _m_impl._root->right_tree_)
                    assert(ptr->color_ == Color::Red);
                free_node(ptr);
                ptr = nullptr;
                return true;
            }
        }
    }

    // 搜索儿子节点中最小节点，并将最小节点的颜色修改为红色
    // 返回旋转与否的 bool 值
    bool search_for_delete_min(Node*& ptr)
    {
        if (ptr == nullptr || ptr->left_tree_ == nullptr) return false;

        bool res = false;
        do
        {
            Node* l_ptr = ptr->left_tree_;
            if (l_ptr->color_ == Color::Red)
                break;  // 如果左子节点已经是红色, 直接返回

            // 如果左子节点的子节点有红色，说明左子节点是3-节点或者4-节点，直接返回
            if (l_ptr->left_tree_ != nullptr  && l_ptr->left_tree_->color_ == Color::Red ||
                l_ptr->right_tree_ != nullptr && l_ptr->right_tree_->color_ == Color::Red)
                break;

            // 只要左子节点为黑，那么如果父节点非空，则右子节点必须是非空的, 否则违反了rb树的性质
            Node* r_ptr = ptr->right_tree_;
            assert(r_ptr != nullptr);

            //父节点可能是黑色的情况，因为有可能兄弟节点T是红色，需要旋转处理
            if (r_ptr->color_ == Color::Red)
            {
                ptr = SignalRotateRightForRemove(ptr);
                // 需要单独重设颜色
                ptr->color_ = Color::Black;
                Node* n_l_ptr = ptr->left_tree_;
                n_l_ptr->color_ = Color::Black;
                n_l_ptr->left_tree_->color_ = Color::Red;
                n_l_ptr->right_tree_->color_ = Color::Red;
                res = true;
                break;
            }

            Color r_l_c = get_color(r_ptr->left_tree_);
            Color r_r_c = get_color(r_ptr->right_tree_);
            // 对于根节点，也可以使用该算法进行处理，做复用，但是最后必须将新跟节点的值设置为 Black
            // 父节点是红色，右子树只能是黑色
            if (r_l_c == Color::Black && r_r_c == Color::Black)
            {
                // 将父节点，左右子节点共同组成一个4-节点
                ptr->color_ = Color::Black;
                l_ptr->color_ = Color::Red;
                r_ptr->color_ = Color::Red;
            }
            else if (r_l_c == Color::Red) // r_r_c为红 或 r_r_c 为黑 两种情况
            {
                // 进行双旋转
                ptr = doubleRotateRightForRemove(ptr);
                res = true;
            }
            else // 仅包含 r_r_c 为红，r_l_c 为黑的情况
            {
                // 进行单旋转
                ptr = SignalRotateRightForRemove(ptr);
                ChangeColorAfterRemoveRightRotate(ptr);
                res = true;
            }
        }while(false);
        return res;
    }


    // 搜索儿子节点中最大节点，并将最大节点的颜色修改为红色
    bool search_for_delete_max(Node*& ptr)
    {
        if (ptr == nullptr || ptr->right_tree_ == nullptr) return false;

        bool res = false;
        do
        {
            Node* r_ptr = ptr->right_tree_;
            if (r_ptr->color_ == Color::Red)
                break;  // 如果右子节点已经是红色, 直接返回

            // 如果右子节点的子节点有红色，说明右子节点是3-节点或者4-节点，直接返回
            if (r_ptr->left_tree_ != nullptr  && r_ptr->left_tree_->color_ == Color::Red ||
                r_ptr->right_tree_ != nullptr && r_ptr->right_tree_->color_ == Color::Red)
                break;

            Node* l_ptr = ptr->left_tree_;
            assert(l_ptr != nullptr);

            //父节点可能是黑色的情况，因为有可能兄弟节点T是红色，需要旋转处理
            if (l_ptr->color_ == Color::Red)
            {
                ptr = SignalRotateLeftForRemove(ptr);
                // 需要单独重设颜色
                ptr->color_ = Color::Black;
                Node* n_r_ptr = ptr->right_tree_;
                n_r_ptr->color_ = Color::Black;
                n_r_ptr->left_tree_->color_ = Color::Red;
                n_r_ptr->right_tree_->color_ = Color::Red;
                res = true;
                break;
            }

            Color l_l_c = get_color(l_ptr->left_tree_);
            Color l_r_c = get_color(l_ptr->right_tree_);
            // 父节点是红色，左子树只能是黑色
            if (l_l_c == Color::Black && l_r_c == Color::Black)
            {
                // 将父节点，左右子节点共同组成一个4-节点
                ptr->color_ = Color::Black;
                l_ptr->color_ = Color::Red;
                r_ptr->color_ = Color::Red;
            }
            else if (l_r_c == Color::Red) // l_l_c为红 或 l_l_c 为黑 两种情况
            {
                // 进行双旋转
                ptr = doubleRotateLeftForRemove(ptr);
                res = true;
            }
            else // 仅包含 l_l_c 为红，l_r_c 为黑的情况
            {
                // 进行单旋转
                ptr = SignalRotateLeftForRemove(ptr);
                ChangeColorAfterRemoveLeftRotate(ptr);
                res = true;
            }
        }while(false);
        return res;
    }

#if 0
    enum class NodeType
    {
        THIS,   // 当前节点
        SON,    // 子节点
        G_SON   // 孙节点
    };

    // 自顶向下 递归
    // 开始向下递归时, 分裂4-节点
    // 返回向上递归时, 判断节点连接, 进行旋转
    // 本函数在退出递归时同时考虑处理单旋转和双旋转，代码逻辑过于复杂，停止调试
    using ResType = Pair<Node*, NodeType>;
    ResType insert2(Node* ptr, Node* f_ptr, const T&val)
    {
        if (ptr == nullptr)
        {
            ptr = buy_node(val, Color::Red); // 插入一定是一棵红色节点
            return ResType{ptr, NodeType::G_SON};  // 对于旋转来说，相当于返回孙子节点, 统一处理
        }

        if (get_color(ptr->left_tree_) == Color::Red &&
            get_color(ptr->right_tree_) == Color::Red)
        {
            ptr->color_ = Color::Red;
            ptr->left_tree_->color_ = Color::Black;
            ptr->right_tree_->color_ = Color::Black;
        }

        ResType res{ptr, NodeType::THIS};
        do
        {
            if (alg::gt(ptr->data_, val))
            {
                res = insert2(ptr->left_tree_, ptr, val);
                if (res.first == nullptr) break;

                // 这里的res就相当于ptr 的新下级节点
                if (ptr->color_ == Color::Red && res.first->color_ == Color::Red)
                {
                    // assert(res.first == ptr->left_tree_); // 不可能连续三个节点都是红色的
                    res = balance_check2(res.first, ptr, f_ptr, val);
                }

                switch (res.second)
                {
                    case NodeType::THIS:
                    {
                        break;
                    }
                    case NodeType::SON:
                    {
                        res.second = NodeType::G_SON;
                        break;
                    }
                    case NodeType::G_SON:
                    {
                        ptr->left_tree_ = res.first;
                        res.first = ptr;
                        res.second = NodeType::THIS;
                        break;
                    }
                    default:
                        break;
                }
            }
            else if (alg::le(ptr->data_, val))
            {
                res = insert2(ptr->right_tree_, ptr, val);
                if (res.first == nullptr) break;

                if (ptr->color_ == Color::Red && res.first->color_ == Color::Red)
                {
                    // assert(res.first != ptr->right_tree_); // 不可能连续三个节点都是红色的
                    if (ptr->right_tree_ == nullptr)
                    {
                        // todo 需要考虑如果在双旋转时，pr->right_tree是叶子节点的情况
                    }
                    res = balance_check2(res.first, ptr, f_ptr, val);
                }

                switch (res.second)
                {
                    case NodeType::THIS:
                    {
                        break;
                    }
                    case NodeType::SON:
                    {
                        res.second = NodeType::G_SON;
                        break;
                    }
                    case NodeType::G_SON:
                    {
                        ptr->right_tree_ = res.first;
                        res.first = ptr;
                        res.second = NodeType::THIS;
                        break;
                    }
                    default:
                        break;
                }
            }

        }while(false);

        return res;
    }

    // 返回新的根节点
    ResType balance_check2(Node* ptr, Node* pa, Node* gptr, const T& val)
    {
        ResType res = {nullptr, NodeType::SON};
        assert(gptr->color_ == Color::Black);
        // 左旋转
        if (alg::gt(gptr->data_, ptr->data_))
        {
            if (alg::gt(pa->data_, ptr->data_)) // 单旋转
                res.first = SignalRotateLeft(gptr);
            else
            {
                res.first = doubleRotateLeft(gptr);
                res.second = NodeType::G_SON;
            }
        }
        else // 右旋转
        {
            if (alg::le(pa->data_, ptr->data_)) // 单旋转
                res.first = SignalRotateRight(gptr);
            else
            {
                res.first = doubleRotateRight(gptr);
                res.second = NodeType::G_SON;
            }
        }

        return res;
    }
#endif

    // check p_ptr和 p_pa 是否同时为red节点
    // 将所有的单旋转和双旋转同时进行考虑
    void balance_check(Node* ptr, Node* pa, Node* gptr, Node* _gptr, const T& val)
    {
        if (ptr->color_ == Color::Red && pa->color_ == Color::Red)
        {
            assert(gptr->color_ == Color::Black);
            // 如果曾祖父节点是根节点，那么默认是右边
            // 该判断位于ptr和pa节点为红色的情况下，在程序一开始的过程中，也就是 gptr和_gptr都是
            // 根节点， 是不可能进入到这里的。
            Dir _gptr_dir = Dir::Right;
            if (_gptr != _m_impl._root)
                _gptr_dir = alg::gt(_gptr->data_, gptr->data_) ? Dir::Left : Dir::Right;

            if (alg::gt(gptr->data_, ptr->data_))
            {
                // 左旋转
                if (alg::gt(pa->data_, ptr->data_)) // 单旋转
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

    // 传参为祖父节点指针
    Node* SignalRotateLeft(Node* gptr)
    {
        Node* pa = gptr->left_tree_;
        gptr->left_tree_ = pa->right_tree_;
        pa->right_tree_ = gptr;
        
        gptr->color_ = Color::Red;
        pa->color_ = Color::Black;

        return pa;
    }

    Node* SignalRotateRight(Node* gptr)
    {
        Node* pa = gptr->right_tree_;
        gptr->right_tree_ = pa->left_tree_;
        pa->left_tree_ = gptr;

        gptr->color_ = Color::Red;
        pa->color_ = Color::Black;

        return pa;
    }

    Node* doubleRotateLeft(Node* gptr)
    {
        Node* new_p = SignalRotateRight(gptr->left_tree_);
        gptr->left_tree_ = new_p;
        return SignalRotateLeft(gptr);
    }

    Node* doubleRotateRight(Node* gptr)
    {
        Node* new_p = SignalRotateLeft(gptr->right_tree_);
        gptr->right_tree_ = new_p;
        return SignalRotateRight(gptr);
    }

    // 专门用于删除时进行旋转, 不修改任何颜色，后面集中处理
    Node* SignalRotateRightForRemove(Node* gptr)
    {
        Node* pa = gptr->right_tree_;
        gptr->right_tree_ = pa->left_tree_;
        pa->left_tree_ = gptr;
        return pa;
    }

    // 针对右单旋转之后的颜色修改
    void ChangeColorAfterRemoveRightRotate(Node* gptr)
    {
        gptr->color_ = Color::Red;
        gptr->left_tree_->left_tree_->color_ = Color::Red;
        gptr->left_tree_->color_ = Color::Black;
        gptr->right_tree_->color_ = Color::Black;
    }

    // 专门用于删除时进行旋转, 不修改任何颜色，后面集中处理
    Node* SignalRotateLeftForRemove(Node* gptr)
    {
        Node* pa = gptr->left_tree_;
        gptr->left_tree_ = pa->right_tree_;
        pa->right_tree_ = gptr;
        return pa;
    }

    // 针对左单旋转之后的颜色修改
    void ChangeColorAfterRemoveLeftRotate(Node* gptr)
    {
        gptr->color_ = Color::Red;
        gptr->right_tree_->right_tree_->color_ = Color::Red;
        gptr->left_tree_->color_ = Color::Black;
        gptr->right_tree_->color_ = Color::Black;
    }

    // 专门用于删除时进行旋转, 包含了颜色的处理
    Node* doubleRotateRightForRemove(Node* gptr)
    {
        Node* new_p = SignalRotateLeftForRemove(gptr->right_tree_);
        gptr->right_tree_ = new_p;
        new_p = SignalRotateRightForRemove(gptr);

        ChangeColorAfterRemoveRightRotate(new_p);

        return new_p;
    }

    // 专门用于删除时进行旋转, 包含了颜色的处理
    Node* doubleRotateLeftForRemove(Node* gptr)
    {
        Node* new_p = SignalRotateRightForRemove(gptr->left_tree_);
        gptr->left_tree_ = new_p;
        new_p = SignalRotateLeftForRemove(gptr);

        ChangeColorAfterRemoveLeftRotate(new_p);

        return new_p;
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
                is_sort &= alg::gt(ptr->data_, ptr->left_tree_->data_);
            if (ptr->right_tree_)
                is_sort &= alg::gt(ptr->right_tree_->data_, ptr->data_);

            res.first = is_sort;
            res.second = l_info.second + (c == Color::Black ? 1 : 0);
        } while (false);

        return res;
    }

    Color get_color(Node* ptr)
    {
        if (ptr == nullptr) return Color::Black;  // 约定默认空节点为黑色

        return ptr->color_;
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
    static Node*  null_node_;
};

template<typename T, typename Alloc>
typename RbTree<T, Alloc>::Node*  RbTree<T, Alloc>::null_node_;

}
}

#endif