#ifndef _TOOLS_BALANCE_TREE_RECURSIVE2_H_
#define _TOOLS_BALANCE_TREE_RECURSIVE2_H_

#include <memory>
#include <cassert>

#include "balance_tree_base.h"
#include "balance_tree_util_base2.h"
#include "balance_tree_print_util.h"

#include "pair.hpp"
#include "algorithm.hpp"

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
    int          size_{0};                 // 数组包含的元素个数
    _BNode*      parent_{nullptr};         // 父指针
    _Entry<T>**  array_{nullptr};          // 指针数组
};

/*B树 包含父指针的递归版本*/
template<typename T,
        typename Alloc = std::allocator<T>,
        template <typename T1> class BNode = _BNode,
        template <typename T2> class BEntry = _Entry>
class BalanceTree : protected BalanceTree_Base<T, Alloc, BNode, BEntry>,
                    protected BalanceTree_Util2<T, BNode, BEntry>
{
    typedef BalanceTree_Base<T, Alloc, BNode, BEntry> BalanceTreeBase;
    typedef typename BalanceTreeBase::Node Node;
    typedef typename BalanceTreeBase::Entry Entry;
    typedef Node*  Root;

    typedef BalanceTree_Util2<T, BNode, BEntry> BalanceUtil;
    typedef typename BalanceUtil::Result Result;
    typedef typename BalanceUtil::Dir    Dir;

public:
    using BalanceTreeBase::buy_node;
    using BalanceTreeBase::free_node;
    using BalanceTreeBase::_m_impl;
    using BalanceTreeBase::buy_entry;
    using BalanceTreeBase::free_entry;
    using BalanceTreeBase::alloc_balance;

    // 算法函数
    using BalanceUtil::lend_ele;
    using BalanceUtil::shift_element;
    using BalanceUtil::shift_node;
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
    using BalanceUtil::is_b_tree;

    // 成员变量
    using BalanceUtil::m_;
    using BalanceUtil::m_half_;
    using BalanceUtil::hight_;

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
    bool insert(const T& val)
    {
        bool change_min_ele = false;
        if (_m_impl._root != nullptr && alg::le(val, _m_impl._root->array_[0]->data_))
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
                new_root->array_[new_root->size_] = buy_entry(_m_impl._root->array_[0]->data_);
                new_root->array_[new_root->size_++]->next_ = _m_impl._root;
                _m_impl._root->parent_ = new_root;
                new_root->array_[new_root->size_] = buy_entry(res.first->array_[0]->data_);
                new_root->array_[new_root->size_++]->next_ = res.first;
                res.first->parent_ = new_root;
                _m_impl._root = new_root;
            }
        }
        else
        {
            if (change_min_ele)
                _m_impl._root->array_[0]->data_ = val;
        }

        ele_size_ ++;

        return true;
    }

    // 自底向上  递归
    bool remove(const T& val)
    {
        RemoveRes res{false, false};
        res = remove(_m_impl._root, 0/*根节点则未用到*/,  val, hight_);
        if (!res.first) return false;

         // 更换根节点， 如果m_为2，3则可能存在单个元素链的场景，需要循环处理
        while (hight_ > 0 && _m_impl._root->size_ == 1)
        {
            Node* ptr = _m_impl._root->array_[0]->next_;
            free_entry(_m_impl._root->array_[0]);
            _m_impl._root->size_ --;
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

    bool memory_alloc_balance() // pass
    {
        return alloc_balance();
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
    /*
        当叶子节点的元素数 < m/2, 则向两边的兄弟节点借, 如果发现两边兄弟节点的元素数均 < m/2,
        则合并两个叶子节点，将元素值较大的节点的元素挪到元素值较小的节点，然后删除前者的节点内存
        非叶子节点的清理策略同叶子节点, 对根节点进行特殊处理
        自底向上的删除
        在递归调用之后检查节点是否需要继续合并，程序处理的方向就是自底向上
    */
    using RemoveRes = Pair<bool /*is remove ele ?*/, bool/*is combine Node struct?*/>;
    // index 为 ptr在pptr中的位置信息
    RemoveRes remove(Node* ptr, int32_t index, const T& val, int32_t hight)
    {
        RemoveRes n_res(false, false);
        if (ptr == nullptr) return n_res;
        bool change_min_ele = false;

        Result f_res{nullptr, 0, false};     // 返回查找返回值
        if (hight > 0)
        {
            f_res = find_next(ptr, val);
            if (f_res.tag_ /*&& alg::eq(val, ptr->array_[0]->data_)*/)
            {
                change_min_ele = true;
            }

            n_res = remove(f_res.ptr_->next_, f_res.i_, val, hight - 1);
            if (!n_res.first)  return n_res;  // 直接返回

            // 对最小值进行重新赋值
            if (change_min_ele &&
                ptr->size_ > f_res.i_ /*&& ptr->array_[f_res.i_]->next_ != nullptr*/)
            {
                assert(ptr->array_[f_res.i_]->next_ != nullptr);
                ptr->array_[f_res.i_]->data_
                    = ptr->array_[f_res.i_]->next_->array_[0]->data_;
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
        if (pptr == nullptr) // 根节点
            return n_res;

        // 先处理节点删除后直接为空的情况, 阶为 2时有这种可能
        if (ptr->size_ == 0)
        {
            // 处理逻辑是，先处理这一层的内容，上面的层数由退出递归时处理
            free_node(ptr);
            // 挪动
            free_entry(pptr->array_[index]);
            // pptr->array_[index] = nullptr;  // 与下面的赋值 nullptr结果相同
            for (int i = index + 1; i < pptr->size_; i++)
            {
                pptr->array_[i - 1] = pptr->array_[i];
            }
            pptr->array_[pptr->size_ - 1] = nullptr;
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

    // 删除完毕，进行重排, 需要回收内存
    void remove_ele(Node* ptr, int32_t index)
    {
        assert(ptr->size_ >= index + 1);
        assert(ptr->array_[index]->next_ == nullptr);  // 必须将待删除的节点信息全部维护完才能删除
        
        free_entry(ptr->array_[index]);

        int i = index + 1;
        for (; i < ptr->size_; i++)
        {
            ptr->array_[i-1] = ptr->array_[i];
        }

        ptr->array_[ptr->size_ - 1] = nullptr;
        ptr->size_ --;
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
    // 需要回收内存
    void handle_merge_node(Node* pptr, int32_t left_index, int32_t right_index, Dir lost_ele_dir)
    {
        assert(pptr->size_ >= right_index);
        shift_node(pptr->array_[left_index]->next_, pptr->array_[right_index]->next_, lost_ele_dir);
        
        free_node(pptr->array_[right_index]->next_);
        free_entry(pptr->array_[right_index]);

        for (int i = right_index + 1; i < pptr->size_; i++)
        {
            pptr->array_[i - 1] = pptr->array_[i];
        }
        pptr->array_[pptr->size_ - 1] = nullptr;
        pptr->size_ --;
    }

    using InsertRes = Pair<Node*, bool>;
    // 在递归调用之后检查节点的分裂，程序处理的方向就是自底向上
    InsertRes insert(Node* ptr, const T& val, int32_t hight)
    {
        InsertRes n_res(nullptr, true);      // 保存下层递归的返回值
        Node* ret_ptr = ptr;                 // 返回的指针
        Result f_res{nullptr, 0, false};     // 返回查找返回值
        bool   change_min_ele = false;
        if (hight > 0)
        {
            f_res = find_next(ptr, val);
            if (alg::le(val, ptr->array_[0]->data_))
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
                    n_res.first->array_[0]->data_ = val;
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
        else  // hight < 0
        {
            assert (ptr == nullptr);
            ret_ptr = buy_node();
            ret_ptr->array_[ret_ptr->size_++] = buy_entry(val);
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
                auto pair = split(ptr, f_res.i_);
                ret_ptr = pair.first;
                if (pair.second)
                {
                    ptr = ret_ptr;
                }
            }
        }
        else
        {
            if (ptr->size_ == m_)
            {
                f_res.i_ += 1;
                auto pair = split(ptr, f_res.i_);
                ret_ptr = pair.first;
                if (pair.second)
                {
                    ptr = ret_ptr;
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
            ptr->array_[f_res.i_] = buy_entry(n_res.first->array_[0]->data_);
            ptr->array_[f_res.i_]->next_ = n_res.first;
            n_res.first->parent_ = ptr;
            // 当下级新增了一个B树最小值时，如果新增了Node, 那么该Node将会插入到
            // 当前层的1号索引位，所以需要将s_node.index + 1，0号索引位还是指向了
            // 之前已有的节点，指向是不需要修改的。但是内部的值在这种情况下还是需要
            // 替换.
            if (change_min_ele)
            {
                ptr->array_[0]->data_ = val;
            }
        }
        else
        {
            ptr->array_[f_res.i_] = buy_entry(val);
        }

        // 应该返回哪一个节点的指针？新增的节点指针或原ptr指针
        n_res.first = ret_ptr;
        return n_res;
    }

    /*
        @note  分裂节点, 返回分裂后Node节点指针, bool 表示是否将插入到返回的Node节点上
        @param index表示将需要向Node中插入的位置，返回后会被程序修改为实际应该插入的位置
    */
    Pair<Node*, bool> split(Node* ptr, int32_t& index)
    {
        assert(ptr->size_ == m_);
        bool m_odd = m_ % 2 == 0 ? false : true;
        bool shift_node = false;   // 是否应该转换节点
        Node* n_ptr = buy_node();
        // m_为奇数时, 根据index的值来选择迁移元素的数目，保证最终插入后两边数量一致
        if (m_odd)
        {
            // 以m_half_为边界进行划分
            if (index >= m_half_)
            {
                shift_node = true;
                index = index - m_half_;
                n_ptr->size_ = m_ - m_half_;
                int j = 0;
                for (int i = m_half_; i < m_; i++)
                {
                    j = i - m_half_;
                    n_ptr->array_[j] = ptr->array_[i];
                    ptr->array_[i] = nullptr;
                    if (n_ptr->array_[j]->next_ != nullptr)
                    {
                        n_ptr->array_[j]->next_->parent_ = n_ptr;  // 维护父指针
                    }
                }
            }
            else
            {
                n_ptr->size_ = m_ - m_half_ + 1;
                int j = 0;
                for (int i = m_half_ - 1; i < m_; i++)
                {
                    j = i - m_half_ + 1;
                    n_ptr->array_[j] = ptr->array_[i];
                    ptr->array_[i] = nullptr;
                    if (n_ptr->array_[j]->next_ != nullptr)
                    {
                        n_ptr->array_[j]->next_->parent_ = n_ptr;  // 维护父指针
                    }
                }
            }
        }
        else
        {
            for (int i = 0; i < m_half_; i++)
            {
                n_ptr->array_[i] = ptr->array_[m_half_ + i];
                ptr->array_[m_half_ + i] = nullptr;
                if (n_ptr->array_[i]->next_ != nullptr)
                {
                    n_ptr->array_[i]->next_->parent_ = n_ptr; // 维护父指针
                }
            }
            n_ptr->size_ = m_ - m_half_;
            if (index > m_half_)
            {
                index = index - m_half_;
                shift_node = true;
            }
        }
        ptr->size_ = m_ - n_ptr->size_;
        n_ptr->parent_ = ptr->parent_;

        return Pair<Node*, bool>{n_ptr, shift_node};
    }

    void destory(Node* ptr)
    {
        if(ptr == nullptr || ptr->size_ == 0) return;

        for(int i = 0; i < ptr->size_; i++)
        {
            destory(ptr->array_[i]->next_);
            free_entry(ptr->array_[i]);
        }
        free_node(ptr);
    }

private:
    std::size_t ele_size_{0}; // 总元素数
};

}
}

#endif