#ifndef _TOOLS_BINARY_RB_TREE_RECURSIVE_H_
#define _TOOLS_BINARY_RB_TREE_RECURSIVE_H_

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

namespace rb_tree_recurs_1
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


/*红黑树 递归版本*/
template<typename T, template <typename T1> class RbNode = _RbNode, typename Alloc = std::allocator<T>>
class RbTree : protected RbTree_Base<T, RbNode, Alloc>
{
    typedef RbTree_Base<T, RbNode, Alloc> RbTreeBase;
    typedef typename RbTreeBase::Node Node;
    typedef Node*  Root;

public:
    using RbTreeBase::buy_node;
    using RbTreeBase::free_node;
    using RbTreeBase::_m_impl;

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

    // 自顶向下插入 递归
    bool insert(const T& val)         // pass
    {
        // insert2的过程有问题，废弃
        Node* ret_ptr = nullptr;
        bool res = insert3(_m_impl._root->right_tree_, val, &ret_ptr);

        ret_ptr->color_ = Color::Black;
        _m_impl._root->right_tree_ = ret_ptr;
        return res;
    }

    // 自顶向下删除 递归
    // 发现了一个神奇的现象，任何删除操作，即使没有找到目标元素，也会导致整个rb_tree的染色发生变化
    // 但依然能保持rb_tree的特性
    bool remove_(const T& val)       // pass
    {
        // 先进行变换， 在结束后，将根节点的颜色变为黑色即可
        bool res = remove(&_m_impl._root->right_tree_, _m_impl._root, Dir::Right, val);
        
        if (_m_impl._root->right_tree_)
            _m_impl._root->right_tree_->color_ = Color::Black;
        return res;
    }

    // 自顶向下删除 递归 推荐使用, 编码过程简单明了
    bool remove(const T& val)         // pass
    {
        handle_before_remove(_m_impl._root->right_tree_, _m_impl._root, val);
        bool res = remove2(&_m_impl._root->right_tree_, _m_impl._root, val);
        
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

    void print_tree()
    {
        draw_tree<Node>(_m_impl._root->right_tree_);
    }

private:
    // 自顶向下 递归
    // 开始向下递归时, 分裂4-节点
    // 返回向上递归时, 判断节点连接, 进行旋转
    // 本函数在退出递归时将单旋转和双旋转分开考虑
    bool insert3(Node* ptr, const T&val, Node** ret_pptr)
    {
        if (ptr == nullptr)
        {
            *ret_pptr = buy_node(val, Color::Red);
            return true;
        }

        if (get_color(ptr->left_tree_) == Color::Red &&
            get_color(ptr->right_tree_) == Color::Red)
        {
            ptr->color_ = Color::Red;
            ptr->left_tree_->color_ = Color::Black;
            ptr->right_tree_->color_ = Color::Black;
        }

        bool res = false;     // 插入重复数据返回false
        Node* ret_ptr = ptr;  // 设置为ptr的初值
        do
        {
            if (alg::gt(ptr->data_, val))
            {
                ret_ptr = ptr->left_tree_;
                res = insert3(ptr->left_tree_, val, &ret_ptr);

                // 先将下面返回的指针接到现有的ptr上
                if (ret_ptr != ptr->left_tree_)
                    ptr->left_tree_ = ret_ptr;

                /*关键:  判断是否应该单旋转或者双旋转，ptr必须在顶部位置*/
                if (ret_ptr->color_ == Color::Red)
                {
                    if (ret_ptr->left_tree_ && ret_ptr->left_tree_->color_ == Color::Red)
                    {
                        // 单循环
                        ret_ptr = SignalRotateLeft(ptr);
                    }
                    else if (ret_ptr->right_tree_ && ret_ptr->right_tree_->color_ == Color::Red)
                    {
                        // 双循环
                        ret_ptr = doubleRotateLeft(ptr);
                    }
                }

                // 如果ptr没有发生改变, 那么应该照常返回
                if (ptr->left_tree_ == ret_ptr)
                {
                    ret_ptr = ptr; // 正常返回 ptr
                }
            }
            else if (alg::le(ptr->data_, val))
            {
                ret_ptr = ptr->left_tree_;
                res = insert3(ptr->right_tree_, val, &ret_ptr);

                if (ret_ptr != ptr->right_tree_)
                    ptr->right_tree_ = ret_ptr;

                // 这里的res就相当于ptr 的新下级节点
                if (ret_ptr->color_ == Color::Red)
                {
                    if (ret_ptr->left_tree_ && ret_ptr->left_tree_->color_ == Color::Red)
                    {
                        ret_ptr = doubleRotateRight(ptr);
                    }
                    else if (ret_ptr->right_tree_ && ret_ptr->right_tree_->color_ == Color::Red)
                    {
                        ret_ptr = SignalRotateRight(ptr);
                    }
                }

                if (ptr->right_tree_ == ret_ptr)
                {
                    ret_ptr = ptr; // 正常返回 ptr
                }
            }
        }while(false);

        *ret_pptr = ret_ptr;

        return res;
    }

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

        Dir dir = alg::gt(ptr->data_, val) ? Dir::Left : Dir::Right;

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

    // 自顶向下 递归
    // 这个算法测试通过，方案简单，逻辑清晰，推荐使用和阅读
    // 这个算法的编码过程和下面的remove相比, 最大的改进就是将搜索过程中的目标子树全部旋转为红色后再向下继续搜索
    // 而下面的remove算法的编码过程，将目标子树的向下搜索过程和旋转混在一起，导致出现的情况非常多，异常复杂
    bool remove2(Node** _ptr, Node* pptr, const T& val)
    {
        Node* ptr = *_ptr;
        if (ptr == nullptr) return false;

        bool res = false;
        Node* ret_ptr = nullptr;
        if (alg::gt(ptr->data_, val))
        {
            // 根据是否旋转，判断下一个值应该如何传递，同时保证引用形参的正确性
            res = search_for_delete_min2(ptr, val, false);
            ret_ptr = ptr;
            if (res)
            {
                pptr = ptr->left_tree_;
                ptr = ptr->left_tree_->left_tree_;
                res = remove2(&ptr, pptr, val);
            }
            else
            {
                pptr = ptr;
                ptr = ptr->left_tree_;
                res = remove2(&ptr, pptr, val);
            }
            pptr->left_tree_ = ptr;
            *_ptr = ret_ptr;
            return res;
        }
        else if (alg::le(ptr->data_, val))
        {
            res = search_for_delete_max2(ptr, val, false);   // 仅依据数据来转向
            ret_ptr = ptr;
            if (res)
            {
                pptr = ptr->right_tree_;  // 更新成ptr最新的父指针
                ptr = ptr->right_tree_->right_tree_;
                res = remove2(&ptr, pptr, val);
            }
            else
            {
                pptr = ptr;
                ptr = ptr->right_tree_;
                res = remove2(&ptr, pptr, val);
            }
            pptr->right_tree_ = ptr;
            *_ptr = ret_ptr;
            return res;
        }
        else
        {
            Node* old_ptr = ptr;
            Dir dir_change = Dir::Unknown; // 控制当前节点与父指针的方向
            if ((ptr->left_tree_ != nullptr && ptr->right_tree_ != nullptr) ||
                ptr->right_tree_ != nullptr)
            {
                // 这里处理之后，会发生遍历线路的拐弯，需要特殊处理
                res = search_for_delete_max2(ptr, val, true, true); // 强制转向
                ret_ptr = ptr;
                if (res)
                {
                    pptr = ptr->right_tree_;
                    ptr = ptr->right_tree_->right_tree_;
                }
                else
                {
                    pptr = ptr;
                    ptr = ptr->right_tree_;
                }

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
                        pptr = ptr->left_tree_;  // 更新父指针
                        ptr = ptr->left_tree_->left_tree_;
                    }
                    else
                    {
                        if (dir_change == Dir::Unknown)
                            dir_change = Dir::Left;
                        pptr = ptr;
                        ptr = ptr->left_tree_;
                    }
                }
                old_ptr->data_ = ptr->data_;

                // 将删除目标设置为ptr节点
                res = remove2(&ptr, pptr, ptr->data_);
                // 最后还是更新父节点的左子树，左子树为更新后的值
                if (dir_change == Dir::Unknown)
                    pptr->right_tree_ = ptr;
                else
                    pptr->left_tree_ = ptr;
                *_ptr = ret_ptr;
                return res;
            }
            else if (ptr->left_tree_ != nullptr)
            {
                res = search_for_delete_min2(ptr, val, true, true);
                ret_ptr = ptr;
                if (res)
                {
                    pptr = ptr->left_tree_;  // 更新成ptr最新的父指针
                    ptr = ptr->left_tree_->left_tree_;
                }
                else
                {
                    pptr = ptr;
                    ptr = ptr->left_tree_;
                }

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
                        pptr = ptr->right_tree_;
                        ptr = ptr->right_tree_->right_tree_;
                    }
                    else
                    {
                        if (dir_change == Dir::Unknown)
                            dir_change = Dir::Right;
                        pptr = ptr;
                        ptr = ptr->right_tree_;
                    }
                }
                old_ptr->data_ = ptr->data_;
                // 将删除目标设置为ptr节点
                res = remove2(&ptr, pptr, ptr->data_);
                    // 最后还是更新父节点的左子树，左子树为更新后的值
                if (dir_change == Dir::Unknown)
                    pptr->left_tree_ = ptr;
                else
                    pptr->right_tree_ = ptr;
                *_ptr = ret_ptr;
                return res;
            }
            else
            {
                if (ptr != _m_impl._root->right_tree_)
                    assert(ptr->color_ == Color::Red);
                free_node(ptr);
                ptr = nullptr;
                *_ptr = nullptr;
                return true;
            }
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

            Dir dir = alg::gt(l_ptr->data_, val) ? Dir::Left : Dir::Right;
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

            Dir dir = alg::gt(r_ptr->data_, val) ? Dir::Left : Dir::Right;
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

    // 自顶向下，递归
    // 本质是: 以删除最小节点为目的的搜索函数
    // 该算法没有贯彻下游节点变为红色才向下搜索的原则，导致中间出现非常多复杂场景，虽然调试成功
    // 但是内容混乱，并非一个很好的算法
    bool remove(Node** _ptr, Node* pptr, Dir dir, const T& val)
    {
        Node* ptr = *_ptr;
        if (ptr == nullptr) return false;

        Node* ret_ptr = nullptr; // 记录应该返回给上一层递归的ptr指针
        bool res = false;
        if (alg::gt(ptr->data_, val))
        {
            // 根据是否旋转，判断下一个值应该如何传递，同时保证引用形参的正确性
            if (search_for_delete_min(ptr, pptr, dir, ret_ptr))
            {
                pptr = ptr->left_tree_;
                ptr = ptr->left_tree_->left_tree_;
                res = remove(&ptr, pptr, Dir::Left, val);
            }
            else
            {
                pptr = ptr;
                ptr = ptr->left_tree_;
                res = remove(&ptr, pptr, Dir::Left, val);
            }
            pptr->left_tree_ = ptr;
            *_ptr = ret_ptr;
            return res;
        }
        else if (alg::le(ptr->data_, val))
        {
            if (search_for_delete_max(ptr, pptr, dir, ret_ptr))
            {
                pptr = ptr->right_tree_;  // 更新成ptr最新的父指针
                ptr = ptr->right_tree_->right_tree_;
                res = remove(&ptr, pptr, Dir::Right, val);
            }
            else
            {
                pptr = ptr;
                ptr = ptr->right_tree_;
                res = remove(&ptr, pptr, Dir::Right, val);
            }
            pptr->right_tree_ = ptr;
            *_ptr = ret_ptr;
            return res;
        }
        else
        {
            Node* old_ptr = ptr;
            Dir dir_change = Dir::Unknown; // 控制当前节点与父指针的方向
            if ((ptr->left_tree_ != nullptr && ptr->right_tree_ != nullptr) ||
                ptr->right_tree_ != nullptr)
            {
                if (search_for_delete_max(ptr, pptr, dir, ret_ptr))
                {
                    pptr = ptr->right_tree_;
                    ptr = ptr->right_tree_->right_tree_;
                }
                else
                {
                    pptr = ptr;
                    ptr = ptr->right_tree_;
                }

                // 但是下面的_ptr指针如果被修改，就需要上层的节点知道然后进行重连接，就是pa指针的任务
                while(ptr->left_tree_)
                {
                    Node* tmp = nullptr;
                    if (search_for_delete_min(ptr, pptr, dir_change == Dir::Unknown ? Dir::Right : Dir::Left, tmp)) // ptr
                    {
                        if (dir_change == Dir::Unknown)
                        {
                            // 不相等时，search_for函数内部已经完成了关联
                            if(tmp == ptr)
                                pptr->right_tree_ = ptr;
                            dir_change = Dir::Left;
                        }
                        else
                        {
                            if(tmp == ptr)
                                pptr->left_tree_ = ptr;
                        }
                        pptr = ptr->left_tree_;  // 更新父指针
                        ptr = ptr->left_tree_->left_tree_;
                    }
                    else
                    {
                        if (dir_change == Dir::Unknown)
                            dir_change = Dir::Left;
                        pptr = ptr;
                        ptr = ptr->left_tree_;
                    }
                }
                old_ptr->data_ = ptr->data_;

                // 将删除目标设置为ptr节点
                res = remove(&ptr, pptr, dir_change == Dir::Unknown ? Dir::Right : Dir::Left, ptr->data_);
                // 最后还是更新父节点的左子树，左子树为更新后的值
                if (dir_change == Dir::Unknown)
                    pptr->right_tree_ = ptr;
                else
                    pptr->left_tree_ = ptr;
                *_ptr = ret_ptr;
                return res;
            }
            else if (ptr->left_tree_ != nullptr)
            {
                if (search_for_delete_min(ptr, pptr, dir, ret_ptr))
                {
                    pptr = ptr->left_tree_;  // 更新成ptr最新的父指针
                    ptr = ptr->left_tree_->left_tree_;
                }
                else
                {
                    pptr = ptr;
                    ptr = ptr->left_tree_;
                }

                while(ptr->right_tree_) // 当右子节点存在时，再进行接下来的处理
                {
                    Node* tmp = nullptr;
                    if (search_for_delete_max(ptr, pptr, dir_change == Dir::Unknown ? Dir::Left : Dir::Right, tmp))//
                    {
                        if (dir_change == Dir::Unknown)
                        {
                            if(tmp == ptr)
                                pptr->left_tree_ = ptr;
                            dir_change = Dir::Right;
                        }
                        else
                        {
                            if(tmp == ptr)
                                pptr->right_tree_ = ptr; // 更新变更的ptr指针的父节点
                        }
                        pptr = ptr->right_tree_;
                        ptr = ptr->right_tree_->right_tree_;
                    }
                    else
                    {
                        if (dir_change == Dir::Unknown)
                            dir_change = Dir::Right;
                        pptr = ptr;
                        ptr = ptr->right_tree_;
                    }
                }
                old_ptr->data_ = ptr->data_;
                // 将删除目标设置为ptr节点
                res = remove(&ptr, pptr, dir_change == Dir::Unknown ? Dir::Left : Dir::Right, ptr->data_);
                    // 最后还是更新父节点的左子树，左子树为更新后的值
                if (dir_change == Dir::Unknown)
                    pptr->left_tree_ = ptr;
                else
                    pptr->right_tree_ = ptr;
                *_ptr = ret_ptr;
                return res;
            }
            else
            {
                if (ptr != _m_impl._root->right_tree_)
                    assert(ptr->color_ == Color::Red);
                free_node(ptr);
                ptr = nullptr;
                *_ptr = nullptr;
                return true;
            }
        }
    }

    // 需要额外处理父节点为黑色，如父节点是根节点的情况
    // 直接通过测试，无需修改返回true, 否则为false
    bool check_parent_and_right_child(Node*& ptr)
    {
        if (ptr != nullptr && ptr->right_tree_->color_ == Color::Red)
        {
            ptr->right_tree_->color_ = Color::Black;
            ptr = SignalRotateRightForRemove(ptr); 
            ptr->left_tree_->color_ = Color::Red;

            return false;
        }
        return true;
    }

    // 搜索儿子节点中最小节点，并将最小节点的颜色修改为红色
    // 返回旋转与否的 bool 值
    // 并修改ret_ptr为新根的值，作为返回给上游递归的值
    // ptr为后续继续进行处理的新根
    bool search_for_delete_min(Node*& ptr, Node* pptr, Dir dir, Node*& ret_ptr)
    {
        ret_ptr = ptr;
        if (ptr == nullptr || ptr->left_tree_ == nullptr) return false;

        bool res = false;
        bool check = false;
        for(;;)
        {
            Node* l_ptr = ptr->left_tree_;
            if (l_ptr->color_ == Color::Red)
                break;  // 如果左子节点已经是红色, 直接返回

            if (!check_parent_and_right_child(ptr))
            {
                // 更新ptr和上级节点的连接, ptr为引用传参
                dir == Dir::Right ? pptr->right_tree_ = ptr : pptr->left_tree_ = ptr;
                ret_ptr = ptr;            // 必须在第一次旋转后就将ret_ptr记下
                ptr = ptr->left_tree_;
                check = true;
                continue;
            }

            // 如果左子节点的子节点有红色，说明左子节点是3-节点或者4-节点，直接返回
            if (l_ptr->left_tree_ != nullptr  && l_ptr->left_tree_->color_ == Color::Red ||
                l_ptr->right_tree_ != nullptr && l_ptr->right_tree_->color_ == Color::Red)
                break;

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
                if (check)
                {
                    dir == Dir::Right ? pptr->right_tree_->left_tree_ = ptr : pptr->left_tree_->left_tree_ = ptr;
                }
                else
                    ret_ptr = ptr;
                res = true;
            }
            else // 仅包含 r_r_c 为红，r_l_c 为黑的情况
            {
                // 进行单旋转
                ptr = SignalRotateRightForRemove(ptr);
                ChangeColorAfterRemoveRightRotate(ptr);
                if (check)
                {
                    dir == Dir::Right ? pptr->right_tree_->left_tree_ = ptr : pptr->left_tree_->left_tree_ = ptr;
                }
                else
                    ret_ptr = ptr;
                res = true;
            }
            break;
        }
        return res;
    }

    // 这种场景表示父节点为黑色，但是兄弟节点的颜色为红色。
    // 比如根节点为黑色，但是兄弟节点为红色
    // 简单地将父节点和兄弟节点进行旋转，然后调换颜色即可
    bool check_parent_and_left_child(Node*& ptr)
    {
        if (ptr != nullptr && ptr->left_tree_->color_ == Color::Red)
        {
            ptr->left_tree_->color_ = Color::Black; // 先修改左子节点的颜色，这个是未来的根节点
            ptr = SignalRotateLeftForRemove(ptr);
            ptr->right_tree_->color_ = Color::Red;
            return false;
        }
        return true;
    }

    // 搜索儿子节点中最大节点，并将最大节点的颜色修改为红色
    bool search_for_delete_max(Node*& ptr, Node* pptr, Dir dir, Node*& ret_ptr)
    {
        ret_ptr = ptr;
        if (ptr == nullptr || ptr->right_tree_ == nullptr) return false;

        bool res = false;
        bool check = false;
        for(;;)
        {
            Node* r_ptr = ptr->right_tree_;
            if (r_ptr->color_ == Color::Red)
                break;  // 如果右子节点已经是红色, 直接返回

            if (!check_parent_and_left_child(ptr))
            {
                // 无需更新ptr和上级节点的连接
                dir == Dir::Right ? pptr->right_tree_ = ptr : pptr->left_tree_ = ptr;
                ret_ptr = ptr;
                ptr = ptr->right_tree_;
                check = true;
                continue;
            }

            // 如果右子节点的子节点有红色，说明右子节点是3-节点或者4-节点，直接返回
            if (r_ptr->left_tree_ != nullptr  && r_ptr->left_tree_->color_ == Color::Red ||
                r_ptr->right_tree_ != nullptr && r_ptr->right_tree_->color_ == Color::Red)
                break;

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
                if (check)
                {
                    dir == Dir::Right ? pptr->right_tree_->right_tree_ = ptr : pptr->left_tree_->right_tree_ = ptr;
                }
                else
                    ret_ptr = ptr;
                res = true;
            }
            else // 仅包含 l_l_c 为红，l_r_c 为黑的情况
            {
                // 进行单旋转
                ptr = SignalRotateLeftForRemove(ptr);
                ChangeColorAfterRemoveLeftRotate(ptr);
                if (check)
                {
                    dir == Dir::Right ? pptr->right_tree_->right_tree_ = ptr : pptr->left_tree_->right_tree_ = ptr;
                }
                else
                    ret_ptr = ptr;
                res = true;
            }
            break;
        }
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

        // if (ptr->data_ == T(140))
        // {
        //     draw_tree<Node>(ptr);
        // }
        if (!res.first)
        {
            // draw_tree<Node>(ptr);
            //assert(false);
        }

        return res;
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

template<typename T, template <typename T1> class RbNode, typename Alloc>
typename RbTree<T, RbNode, Alloc>::Node*  RbTree<T, RbNode, Alloc>::null_node_;

}
}

#endif