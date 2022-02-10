#ifndef _TOOLS_BINARY_RB_TREE_BASE_H_
#define _TOOLS_BINARY_RB_TREE_BASE_H_

#include <memory>
#include <cassert>

namespace tools
{

enum class Dir
{
    Unknown = 0,
    Left,
    Right
};

enum class Color
{
    Black,
    Red
};

template<typename T, template <typename T1> class RbNode, typename Alloc>
class RbTree_Base
{
public:
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<RbNode<T>> _RbNode_alloc_type;

    typedef typename std::allocator_traits<_RbNode_alloc_type>
        rebind_traits;

    typedef typename rebind_traits::pointer  pointer;

    typedef RbNode<T>        Node;

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

    // 暂时未用
    Node* buy_node(T&& val)  noexcept
    {
        pointer p = rebind_traits::allocate(_m_impl, 1);
        rebind_traits::construct(_m_impl, p, std::forward<T>(val));
        return static_cast<Node*>(p);
    }

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

// Node中不带parent_指针的版本
template<typename T, template <typename T1> class RbNode>
class RbTree_Util
{
    typedef RbNode<T>        Node;
public:
    // 提前处理根节点的特殊情况
    // 需要处理的情况是: 根据要删除的值，来选择将根节点旋转向何方
    void handle_before_remove(Node*& ptr, Node* pptr, const T& val)
    {
        if (ptr == nullptr) return;

        if (ptr->left_tree_ != nullptr && ptr->right_tree_ != nullptr &&
            ptr->left_tree_->color_ == Color::Red && ptr->right_tree_->color_ == Color::Red)
        {
            return;
        }

        Dir dir = alg::le(val, ptr->data_) ? Dir::Left : Dir::Right;

        if (dir == Dir::Right && ptr->left_tree_ != nullptr  &&
            ptr->left_tree_->color_ == Color::Red)
        {
            ptr->left_tree_->color_ = Color::Black;
            ptr = SignalRotateLeftForRemove(ptr);
            pptr->right_tree_ = ptr;
            ptr->right_tree_->color_ = Color::Red;
        }

        if (dir == Dir::Left && ptr->right_tree_ != nullptr &&
            ptr->right_tree_->color_ == Color::Red)
        {
            ptr->right_tree_->color_ = Color::Black;
            ptr = SignalRotateRightForRemove(ptr);
            pptr->right_tree_ = ptr;
            ptr->left_tree_->color_ = Color::Red;
        }
    }

    // 搜索儿子节点中最小节点，并将最小节点的颜色修改为红色
    // 返回旋转与否的 bool 值
    // lock_dir 表示是否锁定方向，锁定表示是否修改向下搜索的方向
    // is_change_dir 表示是否强制修改原本向下搜索的方向
    bool search_for_delete_min2(Node*& ptr, const T& val, bool lock_dir, bool is_change_dir = false)
    {
        if (ptr == nullptr || ptr->left_tree_ == nullptr) return false;

        bool res = false;
        do
        {
            Node* l_ptr = ptr->left_tree_;
            if (l_ptr->color_ == Color::Red)
                break;  // 如果左子节点已经是红色, 直接返回

            if (l_ptr->left_tree_ != nullptr && l_ptr->right_tree_ != nullptr &&
                l_ptr->left_tree_->color_ == Color::Red && l_ptr->right_tree_->color_ == Color::Red)
            {
                break;
            }

            Dir dir = alg::le(val, l_ptr->data_) ? Dir::Left : Dir::Right;
            // 需要根据遍历顺序后续是否转向，来调整遍历的方向

            bool change_dir = is_change_dir || (!lock_dir && dir == Dir::Right);
            // 由数据或者函数参数驱动转向, 由正常到非正常转向，需要干预
            if (l_ptr->left_tree_ != nullptr  && l_ptr->left_tree_->color_ == Color::Red)
            {
                if (change_dir)
                {
                    l_ptr->left_tree_->color_ = Color::Black;
                    l_ptr = SignalRotateLeftForRemove(l_ptr);
                    ptr->left_tree_ = l_ptr;
                    l_ptr->right_tree_->color_ = Color::Red;
                }
                break;
            }

            // 仅由数据驱动转向   由非正常到正常转向，一般不需要干预
            if (l_ptr->right_tree_ != nullptr && l_ptr->right_tree_->color_ == Color::Red)
            {
                if (change_dir)
                    break;

                l_ptr->right_tree_->color_ = Color::Black;
                l_ptr = SignalRotateRightForRemove(l_ptr);
                ptr->left_tree_ = l_ptr;
                l_ptr->left_tree_->color_ = Color::Red;

                break;
            }

            // 只要左子节点为黑，那么如果父节点非空，则右子节点必须是非空的, 否则违反了rb树的性质
            Node* r_ptr = ptr->right_tree_;
            assert(r_ptr != nullptr);

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
    bool search_for_delete_max2(Node*& ptr, const T& val, bool lock_dir, bool is_change_dir = false)
    {
        if (ptr == nullptr || ptr->right_tree_ == nullptr) return false;

        bool res = false;
        do
        {
            Node* r_ptr = ptr->right_tree_;
            if (r_ptr->color_ == Color::Red)
                break;  // 如果右子节点已经是红色, 直接返回

            if (r_ptr->right_tree_ != nullptr && r_ptr->left_tree_ != nullptr &&
                r_ptr->right_tree_->color_ == Color::Red && r_ptr->left_tree_->color_ == Color::Red)
            {
                break;
            }

            Dir dir = alg::le(val, r_ptr->data_) ? Dir::Left : Dir::Right;
            bool change_dir = is_change_dir || (!lock_dir && dir == Dir::Left);

            if (r_ptr->right_tree_ != nullptr && r_ptr->right_tree_->color_ == Color::Red)
            {
                if (change_dir)
                {
                    r_ptr->right_tree_->color_ = Color::Black;
                    r_ptr = SignalRotateRightForRemove(r_ptr);
                    ptr->right_tree_ = r_ptr;
                    r_ptr->left_tree_->color_ = Color::Red;
                }
                break;
            }

            if (r_ptr->left_tree_ != nullptr  && r_ptr->left_tree_->color_ == Color::Red)
            {
                if (change_dir)
                    break;

                r_ptr->left_tree_->color_ = Color::Black;
                r_ptr = SignalRotateLeftForRemove(r_ptr);
                ptr->right_tree_ = r_ptr;
                r_ptr->right_tree_->color_ = Color::Red;

                break;
            }

            Node* l_ptr = ptr->left_tree_;
            assert(l_ptr != nullptr);

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

    Color get_color(Node* ptr)
    {
        if (ptr == nullptr) return Color::Black;  // 约定默认空节点为黑色

        return ptr->color_;
    }

    static int32_t _get_hight(Node* ptr)
    {
        if (ptr == nullptr) return -1;

        return alg::max(_get_hight(ptr->left_tree_), _get_hight(ptr->right_tree_)) + 1;
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
};

// Node中带parent_指针的版本
template<typename T, template <typename T1> class RbNode>
class RbTree_Util2
{
    typedef RbNode<T>        Node;
public:
    // 提前处理根节点的特殊情况
    // 需要处理的情况是: 根据要删除的值，来选择将根节点旋转向何方
    // pptr的参数可以去掉，因为该值只会是 _m_impl.root_, 为了模块化，所以增加pptr参数
    void handle_before_remove(Node*& ptr, Node* pptr, const T& val)
    {
        if (ptr == nullptr) return;

        if (ptr->left_tree_ != nullptr && ptr->right_tree_ != nullptr &&
            ptr->left_tree_->color_ == Color::Red && ptr->right_tree_->color_ == Color::Red)
        {
            return;
        }

        Dir dir = alg::le(val, ptr->data_) ? Dir::Left : Dir::Right;

        if (dir == Dir::Right && ptr->left_tree_ != nullptr  &&
            ptr->left_tree_->color_ == Color::Red)
        {
            ptr->left_tree_->color_ = Color::Black;
            ptr = SignalRotateLeftForRemove(ptr);
            pptr->right_tree_ = ptr;
            ptr->right_tree_->color_ = Color::Red;
        }

        if (dir == Dir::Left && ptr->right_tree_ != nullptr &&
            ptr->right_tree_->color_ == Color::Red)
        {
            ptr->right_tree_->color_ = Color::Black;
            ptr = SignalRotateRightForRemove(ptr);
            pptr->right_tree_ = ptr;
            ptr->left_tree_->color_ = Color::Red;
        }
    }


    // 搜索儿子节点中最小节点，并将最小节点的颜色修改为红色
    // 返回旋转与否的 bool 值
    // lock_dir 表示是否锁定方向，锁定表示是否修改向下搜索的方向
    // is_change_dir 表示是否强制修改原本向下搜索的方向
    bool search_for_delete_min2(Node*& ptr, const T& val, bool lock_dir, bool is_change_dir = false)
    {
        if (ptr == nullptr || ptr->left_tree_ == nullptr) return false;

        bool res = false;
        do
        {
            Node* l_ptr = ptr->left_tree_;
            if (l_ptr->color_ == Color::Red)
                break;  // 如果左子节点已经是红色, 直接返回

            if (l_ptr->left_tree_ != nullptr && l_ptr->right_tree_ != nullptr &&
                l_ptr->left_tree_->color_ == Color::Red && l_ptr->right_tree_->color_ == Color::Red)
            {
                break;
            }

            Dir dir = alg::le(val, l_ptr->data_) ? Dir::Left : Dir::Right;
            // 需要根据遍历顺序后续是否转向，来调整遍历的方向

            bool change_dir = is_change_dir || (!lock_dir && dir == Dir::Right);
            // 由数据或者函数参数驱动转向, 由正常到非正常转向，需要干预
            if (l_ptr->left_tree_ != nullptr  && l_ptr->left_tree_->color_ == Color::Red)
            {
                if (change_dir)
                {
                    l_ptr->left_tree_->color_ = Color::Black;
                    l_ptr = SignalRotateLeftForRemove(l_ptr);
                    ptr->left_tree_ = l_ptr;
                    l_ptr->right_tree_->color_ = Color::Red;
                }
                break;
            }

            // 仅由数据驱动转向   由非正常到正常转向，一般不需要干预
            if (l_ptr->right_tree_ != nullptr && l_ptr->right_tree_->color_ == Color::Red)
            {
                if (change_dir)
                    break;

                l_ptr->right_tree_->color_ = Color::Black;
                l_ptr = SignalRotateRightForRemove(l_ptr);
                ptr->left_tree_ = l_ptr;
                l_ptr->left_tree_->color_ = Color::Red;

                break;
            }

            // 只要左子节点为黑，那么如果父节点非空，则右子节点必须是非空的, 否则违反了rb树的性质
            Node* r_ptr = ptr->right_tree_;
            assert(r_ptr != nullptr);

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

    bool search_for_delete_max2(Node*& ptr, const T& val, bool lock_dir, bool is_change_dir = false)
    {
        if (ptr == nullptr || ptr->right_tree_ == nullptr) return false;

        bool res = false;
        do
        {
            Node* r_ptr = ptr->right_tree_;
            if (r_ptr->color_ == Color::Red)
                break;  // 如果右子节点已经是红色, 直接返回

            if (r_ptr->right_tree_ != nullptr && r_ptr->left_tree_ != nullptr &&
                r_ptr->right_tree_->color_ == Color::Red && r_ptr->left_tree_->color_ == Color::Red)
            {
                break;
            }

            Dir dir = alg::le(val, r_ptr->data_) ? Dir::Left : Dir::Right;
            bool change_dir = is_change_dir || (!lock_dir && dir == Dir::Left);

            if (r_ptr->right_tree_ != nullptr && r_ptr->right_tree_->color_ == Color::Red)
            {
                if (change_dir)
                {
                    r_ptr->right_tree_->color_ = Color::Black;
                    r_ptr = SignalRotateRightForRemove(r_ptr);
                    ptr->right_tree_ = r_ptr;
                    r_ptr->left_tree_->color_ = Color::Red;
                }
                break;
            }

            if (r_ptr->left_tree_ != nullptr  && r_ptr->left_tree_->color_ == Color::Red)
            {
                if (change_dir)
                    break;

                r_ptr->left_tree_->color_ = Color::Black;
                r_ptr = SignalRotateLeftForRemove(r_ptr);
                ptr->right_tree_ = r_ptr;
                r_ptr->right_tree_->color_ = Color::Red;

                break;
            }

            Node* l_ptr = ptr->left_tree_;
            assert(l_ptr != nullptr);

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

    // 专门用于删除时进行旋转, 不修改任何颜色，后面集中处理
    Node* SignalRotateRightForRemove(Node* gptr)
    {
        Node* pa = gptr->right_tree_;
        gptr->right_tree_ = pa->left_tree_;
        pa->left_tree_ = gptr;

        pa->parent_ = gptr->parent_;
        gptr->parent_ = pa;
        if (gptr->right_tree_ != nullptr)
            gptr->right_tree_->parent_ = gptr;

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

        pa->parent_ = gptr->parent_;
        gptr->parent_ = pa;
        if (gptr->left_tree_ != nullptr)
            gptr->left_tree_->parent_ = gptr;

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

    Node* SignalRotateLeft(Node* gptr)
    {
        Node* pa = gptr->left_tree_;
        gptr->left_tree_ = pa->right_tree_;
        pa->right_tree_ = gptr;

        gptr->color_ = Color::Red;
        pa->color_ = Color::Black;
        
        pa->parent_ = gptr->parent_;
        gptr->parent_ = pa;
        if (gptr->left_tree_ != nullptr)
            gptr->left_tree_->parent_ = gptr;

        return pa;
    }

    Node* SignalRotateRight(Node* gptr)
    {
        Node* pa = gptr->right_tree_;
        gptr->right_tree_ = pa->left_tree_;
        pa->left_tree_ = gptr;

        gptr->color_ = Color::Red;
        pa->color_ = Color::Black;

        pa->parent_ = gptr->parent_;
        gptr->parent_ = pa;
        if (gptr->right_tree_ != nullptr)
            gptr->right_tree_->parent_ = gptr;
        
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

    Color get_color(Node* ptr)
    {
        if (ptr == nullptr) return Color::Black;  // 约定默认空节点为黑色

        return ptr->color_;
    }

    static int32_t _get_hight(Node* ptr)
    {
        if (ptr == nullptr) return -1;

        return alg::max(_get_hight(ptr->left_tree_), _get_hight(ptr->right_tree_)) + 1;
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
        return ptr;
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
        return ptr;
    }
};

}

#endif