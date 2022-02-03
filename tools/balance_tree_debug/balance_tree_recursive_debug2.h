#ifndef _TOOLS_BALANCE_TREE_RECURSIVE_DEBUG2_H_
#define _TOOLS_BALANCE_TREE_RECURSIVE_DEBUG2_H_

#include <memory>
#include <cassert>

#include "balance_tree_base_debug.h"
#include "balance_tree_util_base_debug2.h"
#include "balance_tree_print_util_debug.h"

#include "algorithm.hpp"
#include "pair.hpp"

namespace tools
{

namespace b_tree_recursive2
{

template<typename T>
struct _BNode;

template<typename T>
struct _Entry
{
    _Entry(const T& val) : data_(val) {}
    T data_;
    // 省略联合体的类型名和变量名
    union
    {
        _BNode<T>*  next_{nullptr};
        // Item  item;    仅定义到索引层, 不定义具体的数据，叶子节点的next_指针设置为空
    };
};

template<typename T>
struct _BNode
{
    _BNode() = default;
    int          size_{0};          // 数组包含的元素个数
    _BNode*      parent_{nullptr};  // 指向上级父节点的指针
    _Entry<T>*   array_{nullptr};
};


/*B树 增加父指针的递归版本*/
template<typename T,
        typename Alloc = std::allocator<T>,
        template <typename T1> class BNode = _BNode,
        template <typename T2> class BEntry = _Entry>
class BalanceTree : protected BalanceTree_Base<T, Alloc, BNode, BEntry>,
                    protected BalanceTree_Util<T, BNode, BEntry>
{
    typedef BalanceTree_Base<T, Alloc, BNode, BEntry> BalanceTreeBase;
    typedef typename BalanceTreeBase::Node Node;
    typedef typename BalanceTreeBase::Entry Entry;
    typedef Node*  Root;

    typedef BalanceTree_Util<T, BNode, BEntry> BalanceUtil;
    typedef typename BalanceUtil::Result Result;
    typedef typename BalanceUtil::Dir    Dir;
    typedef typename BalanceUtil::IterResult  IterResult;

public:
    using BalanceTreeBase::buy_node;
    using BalanceTreeBase::free_node;
    using BalanceTreeBase::_m_impl;
    // using BalanceTreeBase::buy_entry;
    // using BalanceTreeBase::free_entry;

    // 算法函数
    using BalanceUtil::lend_ele;
    using BalanceUtil::shift_element;
    using BalanceUtil::shift_node;
    using BalanceUtil::remove_ele;
    using BalanceUtil::find;
    using BalanceUtil::find_val;
    using BalanceUtil::find_next;

    // 迭代遍历函数
    using BalanceUtil::first;
    using BalanceUtil::next;
    using BalanceUtil::last;
    using BalanceUtil::prev;
    using BalanceUtil::in_order;
    using BalanceUtil::prev_in_order;

    // 成员变量
    using BalanceUtil::m_;
    using BalanceUtil::m_half_;

public:
    BalanceTree(size_t m/*B树的阶*/)
        : BalanceTreeBase(m > 1 ? m : 2),
          BalanceUtil(m > 1 ? m : 2) // 最小阶为2
    {
    }

    ~BalanceTree()
    {
        destory(_m_impl._root);
    }

    // 自底向上 递归
    bool insert(const T& val)    // pass
    {
        bool change_min_ele = false;
        if (_m_impl._root != nullptr && alg::le(val, _m_impl._root->array_[0].data_))
        {
            change_min_ele = true;
        }

        // 返回指针指向的值大于原有指针指向的值, 所以需要尾加
        auto res = insert(_m_impl._root, val, hight_);
        if (!res.second) return false;

        if (res.first != _m_impl._root)
        {
            hight_ ++;
            if (_m_impl._root == nullptr)
            {
                _m_impl._root = res.first;
            }
            else
            {
                Node* new_root = buy_node();
                new_root->array_[new_root->size_].data_ = _m_impl._root->array_[0].data_;
                new_root->array_[new_root->size_++].next_ = _m_impl._root;
                _m_impl._root->parent_ = new_root;
                new_root->array_[new_root->size_].data_ = res.first->array_[0].data_;
                new_root->array_[new_root->size_++].next_ = res.first;
                res.first->parent_ = new_root;
                _m_impl._root = new_root;
            }
        }
        else
        {
            if (change_min_ele)
                _m_impl._root->array_[0].data_ = val;
        }

        ele_size_ ++;

        return true;
    }

    // 自底向上  递归
    bool remove(const T& val)    // pass
    {
        RemoveRes res{false, false};
        res = remove(_m_impl._root, 0/*根节点则未用到*/,  val, hight_);
        if (!res.first) return false;

         // 更换根节点， 如果m_为2，3则可能存在单个元素链的场景，需要循环处理
        while (hight_ > 0 && _m_impl._root->size_ == 1)
        {
            Node* ptr = _m_impl._root->array_[0].next_;
            free_node(_m_impl._root);
            _m_impl._root = ptr;
            ptr->parent_ = nullptr;
            hight_ --;
        }

        // 删除完最后一个元素时，root节点清理， hight 变为 -1
        if (_m_impl._root->size_ == 0)
        {
            free_node(_m_impl._root);
            _m_impl._root = nullptr;
            hight_ --;
        }

        ele_size_ --;

        return true;
    }


    bool search(const T& val)
    {
        if (_m_impl._root == nullptr) return false;

        Result res = find(_m_impl._root, val, hight_);
        return res.tag_;
    }

    // 返回元素个数
    size_t InOrder()
    {
        return in_order(_m_impl._root, hight_);
    }

    // 返回元素个数
    size_t PrevInOrder()
    {
        return prev_in_order(_m_impl._root, hight_);
    }

    bool is_b_tree()
    {
        return is_b_tree(_m_impl._root, hight_);
    }

    Node* get_root()
    {
        return _m_impl._root;
    }

    size_t size()
    {
        return ele_size_;
    }

    void print_tree1()
    {
        draw_tree1<Node>(_m_impl._root, hight_, m_);
    }

    void print_tree()
    {
        draw_tree<Node>(_m_impl._root, hight_, m_);
    }

private:
    // 自底向上的删除，算法主体和没有父节点的递归版本一致
    using RemoveRes = Pair<bool /*is remove ele ?*/, bool/*is combine Node struct?*/>;
    // index 为ptr在父节点中的位置信息
    RemoveRes remove(Node* ptr, int32_t index, const T& val, int32_t hight)
    {
        RemoveRes n_res(false, false);
        if (ptr == nullptr) return n_res;
        bool change_min_ele = false;

        Result f_res{nullptr, 0, false};     // 返回查找返回值
        if (hight > 0)
        {
            f_res = find_next(ptr, val);
            if (f_res.tag_ /*&& alg::eq(val, ptr->array_[0].data_)*/)
            {
                change_min_ele = true;
            }

            n_res = remove(f_res.ptr_->next_, f_res.i_, val, hight - 1);
            if (!n_res.first)  return n_res;  // 直接返回

            // 对最小值进行重新赋值
            if (change_min_ele &&
                ptr->size_ > 0 &&
                ptr->array_[f_res.i_].next_ != nullptr)
            {
                ptr->array_[f_res.i_].data_
                    = ptr->array_[f_res.i_].next_->array_[0].data_;
            }

            if (!n_res.second || ptr->size_ >= m_half_)
            {
                return n_res;
            }
        }
        else // hight == 0
        {
            f_res = find_val(ptr, val);
            if (!f_res.tag_)
            {
                n_res.first = false;
                return n_res;
            }
            remove_ele(ptr, f_res.i_);
        }
        n_res.first = true;

        Node* pptr = ptr->parent_;
        if (pptr == nullptr)
            return n_res;

        // 先处理节点删除后直接为空的情况, 阶为 2, 3时有这种可能
        if (ptr->size_ == 0)
        {
            // 处理逻辑是，先处理这一层的内容，上面的层数由退出递归时处理
            free_node(ptr);
            // 挪动
            for (int i = index + 1; i < pptr->size_; i++)
            {
                pptr->array_[i - 1] = pptr->array_[i];
            }
            pptr->array_[pptr->size_ - 1].next_ = nullptr;
            pptr->size_--;
            n_res.second = true;
        }
        else if (ptr->size_ < m_half_)
        {
            if (lend_ele(pptr, index))
            {
                n_res.second = false;
            }
            else
            {
                // 处理完节点的合并，暂时无需更新非叶子节点的最小值
                merge_node(pptr, index);
                n_res.second = true;
            }
        }

        return n_res;
    }

    void merge_node(Node* pptr, int32_t index)
    {
        if (pptr == nullptr) return;

        int32_t left_index = 0;
        int32_t right_index = 0;
        Dir lost_ele_dir = Dir::Unknown;
        if (index == 0)
        {
            right_index = index + 1;
            lost_ele_dir = Dir::Left;
        }
        else if (index == pptr->size_ -1)
        {
            left_index = index - 1;
            right_index = index;
            lost_ele_dir = Dir::Right;
        }
        else
        {
            left_index = index;
            right_index = index + 1;
            lost_ele_dir = Dir::Left;
        }
        handle_merge_node(pptr, left_index, right_index, lost_ele_dir);
    }

    // 合并两个叶子节点，将右边的节点合并到左边
    // 当需要左右两个节点均可以合并时，先合并右节点，再合并左节点
    /*
        1. right_index的数据挪到left_index中
        2. 释放right_index的内容
        3. 重排pptr节点的内容，将right_index后面的内容全部向前挪动一位
    */ 
    void handle_merge_node(Node* pptr, int32_t left_index, int32_t right_index, Dir lost_ele_dir)
    {
        assert(pptr->size_ >= right_index);
        shift_node(pptr->array_[left_index].next_, pptr->array_[right_index].next_, lost_ele_dir);
        free_node(pptr->array_[right_index].next_);
        for (int i = right_index + 1; i < pptr->size_; i++)
        {
            pptr->array_[i - 1] = pptr->array_[i];
        }
        pptr->array_[pptr->size_ - 1].next_ = nullptr;
        pptr->size_ --;
    }

    using InsertRes = Pair<Node*, bool>;
    // 自底向上的插入，算法主体和没有父节点的递归版本一致
    InsertRes insert(Node* ptr, const T& val, int32_t hight)
    {
        InsertRes n_res(nullptr, true);      // 保存下层递归的返回值
        Node* ret_ptr = ptr;                 // 返回的指针
        Result f_res{nullptr, 0, false};     // 返回查找返回值
        bool   change_min_ele = false;
        if (hight > 0)
        {
            f_res = find_next(ptr, val);
            if (alg::le(val, ptr->array_[0].data_))
            {
                //只有在val比B树最小值还小的时候, 才会进入该逻辑
                change_min_ele = true;
            }

            n_res = insert(f_res.ptr_->next_, val, hight - 1);
            // 一致则说明下层未发生分裂
            if (n_res.first == f_res.ptr_->next_)
            {
                // 可能下游将最小值更新，所以这里需要再额外判断
                if (change_min_ele)
                {
                    n_res.first->array_[0].data_ = val;
                }
                n_res.first = ret_ptr;
                return n_res;
            }
        }
        else if (hight == 0)
        {
            f_res = find_val(ptr, val);
            if (f_res.tag_)
            {
                n_res.first = ret_ptr;
                n_res.second = false;
                return n_res;   // 数据存在
            }
        }
        else
        {
            assert (ptr == nullptr);
            ret_ptr = buy_node();
            ret_ptr->array_[ret_ptr->size_++].data_ = val;
            n_res.first = ret_ptr;
            return n_res;
        }

        // 需要进行插入处理
        // 需要先分裂，再插入
        // 这里复用ptr, 赋值为需要插入的节点
        if (hight == 0)
        {
            if (ptr->size_ == m_)
            {
                ret_ptr = split(ptr);
                if (f_res.i_ > m_half_ || f_res.i_ == m_ - 1)  // 后面这种场景是处理m_为2的特殊情况
                {
                    f_res.i_ = f_res.i_ - m_half_;  //插入的新位置
                    ptr = ret_ptr;// 重置ptr
                }
            }
        }
        else
        {
            if (ptr->size_ == m_)
            {
                ret_ptr = split(ptr);
                if (f_res.i_ >= m_half_ || f_res.i_ == m_ - 1)  // 后面这种场景是处理m_为2的特殊情况
                {
                    f_res.i_ = f_res.i_ - m_half_;  //插入的新位置
                    f_res.i_ ++;
                    ptr = ret_ptr;// 重置ptr
                }
                else
                {
                    // 如果刚好m_half, 则不需要更新，因为这个位置为空，可以直接插入
                    if (f_res.i_ != m_half_)
                        f_res.i_ ++;
                }
            }
            else
            {
                // 非叶子节点插入时, 可能需要更新, 插入的到上一个节点的位置需要获取
                f_res.i_ ++;
            }
        }

        // ptr->size_ 是需要++的，因为这个时候数量还是之前的数量
        for(int i = ptr->size_ ++; i > f_res.i_; i--)
        {
            ptr->array_[i] = ptr->array_[i - 1];
        }

        if (hight > 0)
        {
            ptr->array_[f_res.i_].data_ = n_res.first->array_[0].data_;
            ptr->array_[f_res.i_].next_ = n_res.first;
            // n_res是下级新增的节点，序维护parent_指针
            n_res.first->parent_ = ptr;

            if (change_min_ele)
            {
                ptr->array_[0].data_ = val;
            }
        }
        else
        {
            ptr->array_[f_res.i_].data_ = val;
        }

        // 应该返回哪一个节点的指针？新增的节点指针或原ptr指针
        n_res.first = ret_ptr;
        return n_res;
    }

    // 分裂节点, 返回分裂后Node节点指针
    Node* split(Node* ptr)
    {
        assert(ptr->size_ == m_);
        bool m_odd = m_ % 2 == 0 ? false : true;
        Node* n_ptr = buy_node();
        // m_为奇数时, 在原节点保留元素的少, 迁移的元素较多
        int i = 0;
        for (; i < m_half_; i++)
        {
            n_ptr->array_[i] = ptr->array_[m_half_ + i];
            ptr->array_[m_half_ + i].next_ = nullptr;
            ptr->array_[m_half_ + i].data_ = T();
            if (n_ptr->array_[i].next_ != nullptr)
            {
                n_ptr->array_[i].next_->parent_ = n_ptr;  // 维护父指针
            }
        }
        if (m_odd)
        {
            n_ptr->array_[i] = ptr->array_[m_half_ + i];
            ptr->array_[m_half_ + i].next_ = nullptr;
            ptr->array_[m_half_ + i].data_ = T();
            if (n_ptr->array_[i].next_ != nullptr)
            {
                n_ptr->array_[i].next_->parent_ = n_ptr;   // 维护父指针
            }
        }

        n_ptr->size_ = m_ - m_half_;
        ptr->size_ = m_half_;
        return n_ptr;
    }

    // 判断是否为一棵符合规范的B树
    /*
        1. 有序
        2. 节点元素数 m/2 <= x <= m, 根除外（2 <= x <= m）
        3. 所有叶子节点等高
        4. 非叶子节点的元素均需要出现在叶子节点中
        5. m个元素的非叶子节点下游存在m个节点
        6. 非叶子节点中保存的元素值均为下层叶子节点中最小值
    */
    bool is_b_tree(Node* ptr, int hight)
    {
        if (ptr == nullptr)
        {
            assert(hight == -1);
            return true;  //空节点默认是B数
        }
        if (hight_ == 0 && ptr && ptr->size_ == 0)
            return true;   // 删除全部数据后，空节点也算是

        assert(hight >= 0);
        assert(ptr->size_ <= m_);
        assert(ptr->size_ > 0);

        bool res = false;
        if (hight_ > 0)
        {
            if (hight == hight_)
            {
                if (ptr->size_ < 2)  // 根节点的元素数
                    return res;
            }
            else
            {
                if (ptr->size_ < m_half_)
                    return res;
            }
        }

        do
        {
            int j = 0;
            int i = 0;
            for (; i < ptr->size_; i++)
            {
                //assert(ptr->array_[i] != nullptr);
                if (hight > 0)
                {
                    assert(ptr->array_[i].next_ != nullptr);
                    if (!alg::eq(ptr->array_[i].data_, ptr->array_[i].next_->array_[0].data_))
                        break;
                    
                    if (i > 0)
                    {
                        int n_size = ptr->array_[j].next_->size_;
                        // 当前的值比前一位值的下级节点的最大值还要大，保证有序
                        if (!alg::le(ptr->array_[j].next_->array_[n_size - 1].data_, ptr->array_[i].data_))
                        {
                            break;
                        }
                    }
                }
                if (i > 0)
                {
                    if (!alg::gt(ptr->array_[i].data_, ptr->array_[j].data_))
                        break;
                }
                j = i; // 让j落后i一位

                if (!is_b_tree(ptr->array_[i].next_, hight - 1))
                    break;
            }

            if (i < ptr->size_)
                break;
            res = true;
        }while(false);

        if (!res)
        {
            // draw_tree<Node>(ptr, hight, m_);
        }

        return res;
    }

    void destory(Node* ptr)
    {
        if(ptr == nullptr || ptr->size_ == 0) return;

        for(int i = 0; i < ptr->size_; i++)
        {
            destory(ptr->array_[i].next_);
            //free_entry(ptr->array_[i]);
        }
        free_node(ptr);
    }

private:
    int         hight_{-1};  // 树高, 单节点的B树默认树高为0, 空树默认树高为-1
    std::size_t ele_size_{0}; // 总元素数
};

}
}

#endif