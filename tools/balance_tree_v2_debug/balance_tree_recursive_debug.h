#ifndef _TOOLS_BALANCE_TREE_RECURSIVE_V2_DEBUG_H_
#define _TOOLS_BALANCE_TREE_RECURSIVE_V2_DEBUG_H_

#include <memory>
#include <cassert>

#include "../balance_tree_debug/balance_tree_base_debug.h"
#include "balance_tree_util_base_debug.h"
#include "balance_tree_print_util_debug.h"

#include "pair.hpp"
#include "algorithm.hpp"

namespace tools
{

namespace b_tree_recursive_v2
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
    int            size_{0};                 // 数组包含的元素个数
    //_Entry<T>**  array_{nullptr};          // 指针数组 调试不容易,等程序稳定后再更换数据结构
    _Entry<T>*  array_{nullptr};             // 普通Entry数组, 数组个数为B树的阶数M
                                             // 并非传统教科书中的M+1
};

/*
    3阶B树内存布局为：
    （每个Node中array_数组大小为3，数组第size_+1个结构的data_域未用，用 -1 代替.
      用竖线和竖斜线表示指针的指向以及指针和data_域的相对位置，每个Node中array_[size_]
      的位置没有保存data_, 但是保存了尾部的next_指针，需要特殊处理）

    两个星号以及之间的数据表示Entry结构体

    插入 75 前，size_ = 2
                     *ptr|2*  * ptr|53*  *ptr|-1*
                      /          |          \
                
    插入 75 后, 每个节点size_ = 1

                       *ptr|53*   *ptr|-1*
                        /           \ 

             *ptr|2*  *ptr|-1*    *ptr|75*  *ptr|-1*
              /         \          /           \
*/

/*严蔚敏版B树 递归版本*/
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

public:
    using BalanceTreeBase::buy_node;
    using BalanceTreeBase::free_node;
    using BalanceTreeBase::_m_impl;
    // using BalanceTreeBase::buy_entry;
    // using BalanceTreeBase::free_entry;

    // 算法函数
    // using BalanceUtil::lend_ele;
    // using BalanceUtil::shift_element;
    // using BalanceUtil::shift_node;
    // using BalanceUtil::remove_ele;

    using BalanceUtil::find;
    using BalanceUtil::find_val;
    using BalanceUtil::find_next;

    // 成员变量
    using BalanceUtil::m_;
    using BalanceUtil::m_half_;

public:
    BalanceTree(size_t m/*B树的阶*/)
        : BalanceTreeBase(m > 2 ? m : 3),
          BalanceUtil(m > 2 ? m : 3) // 最小阶为3
    {
    }

    ~BalanceTree()
    {
        destory(_m_impl._root);
    }


    bool insert(const T& val)
    {
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
                new_root->array_[new_root->size_].data_ = get_last_data_in_node(_m_impl._root);
                _m_impl._root->size_--;
                new_root->array_[new_root->size_++].next_ = _m_impl._root;
                new_root->array_[new_root->size_].next_ = res.first;
                _m_impl._root = new_root;
            }
        }

        ele_size_ ++;

        return true;
    }

    bool remove(const T& val)
    {

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

    void print_tree()
    {
        draw_tree<Node>(_m_impl._root, hight_, m_);
    }

private:
    T& get_last_data_in_node(Node* ptr)
    {
        assert(ptr != nullptr && ptr->size_ > 0);
        return ptr->array_[ptr->size_ - 1].data_;
    }

    /*
        自底向上的算法 递归
        算法过程是当节点插入前数目等于m-1, 则先分裂，然后再由上级节点进行插入
    */
    // 可以省略hight参数，改由next_指针来判断是否为叶子节点，但考虑后面Item节点的扩展
    // 仍然携带该参数
    using InsertRes = Pair<Node*, bool>;
    InsertRes insert(Node* ptr, const T& val, int32_t hight)
    {
        InsertRes n_res{nullptr, true}; // 递归返回的结果
        Node* ret_ptr = ptr;
        Result f_res{nullptr, 0, false};     // 返回查找返回值
        if (hight > 0)
        {
            f_res = find_next(ptr, val);
            if (f_res.tag_)
            {
                n_res.first = ret_ptr;
                n_res.second = false;
                return n_res;   // 数据存在
            }
            n_res = insert(f_res.ptr_->next_, val, hight - 1);
            // 说明下层没有发生分裂
            if (n_res.first == f_res.ptr_->next_)
            {
                n_res.first = ret_ptr;
                n_res.second = true;
                return n_res;
            }
            // 否则需要旋转处理
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
            ret_ptr->array_[ret_ptr->size_++].data_ = val;
            n_res.first = ret_ptr;
            return n_res;
        }

        if (hight == 0)
        {
            if (ptr->size_ == m_-1)
            {
                auto pair = split(ptr, f_res.i_);
                ret_ptr = pair.first;
                if (pair.second)
                {
                    ptr = ret_ptr;
                }
            }

            // 必须是前置++, 因为要拷贝ptr->size_后面的next_指针域或者未来可能的Item域
            for(int i = ++ptr->size_; i > f_res.i_; i--)
            {
                ptr->array_[i] = ptr->array_[i - 1];
            }
            ptr->array_[f_res.i_].data_ = val;
            /*
                如果实际使用了Item对象，则需要根据splite中各种情况重新维护叶子节点对下的对象，
                就像非叶子节点所做的那样
                目前全为nullptr, 可不去处理
            */
        }
        else
        {
            Node* old_ptr = ptr;
            Node* next_node = f_res.ptr_->next_; // 需要先获取
            if (ptr->size_ == m_-1)
            {
                auto pair = split(ptr, f_res.i_);
                ret_ptr = pair.first;
                if (pair.second)
                {
                    ptr = ret_ptr;
                }
            }

            /*
                这里犯了刻舟求剑的错误，f_res.i_已经过各种修改，但是还直接获取f_res.ptr_的指针
                需要提前获取
                // Node* next_node = f_res.ptr_->next_;
            */

            assert(next_node != nullptr);
            /*
                f_res.i_为 m_half_-1是分裂节点时特殊情况, 拼接需要依赖ptr和ret_ptr两个对象
                如果是其他情况，则需要先将f_res.i_的位置腾挪出来，再做赋值处理
            */
            if (f_res.i_ == m_half_ - 1 && ptr != ret_ptr)
            {
                ptr->size_ ++;

                no_leaf_node_add_ele(ptr, f_res.i_, next_node);
                ret_ptr->array_[0].next_ = n_res.first;
            }
            else 
            {
                if (ptr->size_ > 0)
                {
                    // 必须是前置++, 因为要拷贝ptr->size_后面的next_指针
                    for(int i = ++ptr->size_; i > f_res.i_; i--)
                    {
                        ptr->array_[i] = ptr->array_[i - 1];
                    }

                    no_leaf_node_add_ele(ptr, f_res.i_, next_node);
                    ptr->array_[f_res.i_].next_ = ptr->array_[f_res.i_ + 1].next_;
                    ptr->array_[f_res.i_ + 1].next_ = n_res.first;
                }
                else // ptr->size_ == 0
                {
                    ptr->size_ ++;
                    assert(ptr != old_ptr);
                    assert(f_res.i_ == 0);
                    //split中已经将尾部的next_node指针拷贝到 ret_ptr中
                    assert(ptr->array_[0].next_ == next_node);

                    no_leaf_node_add_ele(ptr, 0, next_node);
                    ptr->array_[ptr->size_].next_ = n_res.first;
                }
            }
        }

        n_res.first = ret_ptr;
        return n_res;
    }

    // 将next_node的最后一个元素的值赋值到ptr指针指向Node的index位置
    void no_leaf_node_add_ele(Node* ptr, int32_t index, Node*next_node)
    {
        ptr->array_[index].data_ = get_last_data_in_node(next_node);
        // 因为这个Entry很快要脱离索引范围, 所以进行清理
        get_last_data_in_node(next_node) = T();
        // 保证之前的操作彻底，不要残留数据
        assert(next_node->array_[next_node->size_].next_ == nullptr);
        next_node->size_ --;
    }

    /*
        @note  分裂节点, 返回分裂后Node节点指针, bool 表示是否将插入到返回的Node节点上
        @param index表示将需要向Node中插入的位置，返回后会被程序修改为实际应该插入的位置
    */
    Pair<Node*, bool> split(Node* ptr, int32_t& index)
    {
        assert(ptr->size_ == m_ - 1);
        bool shift_node = false;   // 是否应该转换节点
        Node* n_ptr = buy_node();

        // 以m_half - 1作为区分点
        if (index > m_half_ -1)
        {
            // 修改最终index的值
            index = index - m_half_;
            shift_node = true;
            // 包含 i == ptr->size_ 还有一个尾部的next_指针
            int i = m_half_;
            for (; i <= ptr->size_; i++)
            {
                n_ptr->array_[i - m_half_] = ptr->array_[i];
                ptr->array_[i].data_ = T();
                ptr->array_[i].next_ = nullptr;
            }
            n_ptr->size_ = ptr->size_ - m_half_;
        }
        else if (index < m_half_ -1)
        {
            // 包含 i == ptr->size_ 还有一个尾部的next_指针
            int i = m_half_ - 1;
            for ( ; i <= ptr->size_; i++)
            {
                n_ptr->array_[i - m_half_ + 1] = ptr->array_[i];
                ptr->array_[i].data_ = T();
                ptr->array_[i].next_ = nullptr;
            }
            n_ptr->size_ = ptr->size_ - m_half_ + 1;
        }
        else // index == m_half_ -1
        {
            int i = m_half_ - 1;
            // 特殊处理：只转移值，不转移指针
            n_ptr->array_[i - m_half_ + 1].data_ = ptr->array_[i].data_;
            ptr->array_[i].data_ = T();
            i++;
            // 包含 i == ptr->size_ 还有一个尾部的next_指针
            for (; i <= ptr->size_; i++)
            {
                n_ptr->array_[i - m_half_ + 1] = ptr->array_[i];
                ptr->array_[i].data_ = T();
                ptr->array_[i].next_ = nullptr;
            }
            n_ptr->size_ = ptr->size_ - m_half_ + 1;
        }

        ptr->size_ = m_ - 1 - n_ptr->size_;

        return Pair<Node*, bool>{n_ptr, shift_node};
    }

    bool remove()
    {

    }

    // 判断是否为一棵符合规范的B树
    /*
        1. 有序
        2. 节点元素数 m/2(上确界)-1 <= x <= m-1, 根除外（1 <= x <= m-1）
        3. 所有叶子节点等高
        4. m个元素的非叶子节点下游存在m+1个节点
        5. 非叶子节点中保存的元素值均比所有左下方元素值大，比所有右下方元素值小
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
        assert(ptr->size_ < m_);
        assert(ptr->size_ > 0);

        bool res = false;
        if (hight_ > 0)
        {
            if (hight != hight_)
            {
                if (ptr->size_ < m_half_ - 1)  // m_half_取得是上确界
                    return res;
            }
        }

        do
        {
            int i = 0;
            int j = 0;
            for (; i < ptr->size_; i++)
            {
                //assert(ptr->array_[i] != nullptr);
                if (hight > 0)
                {
                    assert(ptr->array_[i].next_ != nullptr);

                    if (i > 0)
                    {
                        int n_size = ptr->array_[i].next_->size_;
                        // 当前的值比前一位值的下级节点的最大值还要大，保证有序
                        if (!alg::le(ptr->array_[i].next_->array_[n_size - 1].data_, ptr->array_[i].data_))
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
                j = i; // j落后i一位

                if (!is_b_tree(ptr->array_[i].next_, hight - 1))
                    break;
            }

            if (i < ptr->size_)
                break;

            if (!is_b_tree(ptr->array_[ptr->size_].next_, hight - 1)) // 仅检查size_+1个指针的指向
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

        for(int i = 0; i <= ptr->size_; i++)
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