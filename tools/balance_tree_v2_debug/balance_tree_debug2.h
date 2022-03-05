#ifndef _TOOLS_BALANCE_TREE_V2_DEBUG2_H_
#define _TOOLS_BALANCE_TREE_V2_DEBUG2_H_

#include <memory>
#include <cassert>

#include "../balance_tree_debug/balance_tree_base_debug.h"
#include "balance_tree_util_base_debug2.h"
#include "balance_tree_print_util_debug.h"

#include "pair.hpp"
#include "algorithm.hpp"

namespace tools
{

namespace b_tree_v2_2
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
    _BNode*        parent_{nullptr};         // 父指针
    //_Entry<T>**  array_{nullptr};          // 指针数组 调试不容易,等程序稳定后再更换数据结构
    _Entry<T>*     array_{nullptr};
};


/*严蔚敏版B树 带parent_指针的非递归版本*/
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
    using BalanceTreeBase::buy_array;
    using BalanceTreeBase::free_array;
    // using BalanceTreeBase::buy_entry;
    // using BalanceTreeBase::free_entry;

    // 算法函数
    using BalanceUtil::get_last_data_in_node;
    using BalanceUtil::get_last_ele_from_leaf;
    using BalanceUtil::lend_ele;
    using BalanceUtil::shift_element;
    using BalanceUtil::no_leaf_node_add_ele;
    using BalanceUtil::remove_ele;

    using BalanceUtil::find;
    using BalanceUtil::find_val;
    using BalanceUtil::find_next;
    using BalanceUtil::in_order;
    using BalanceUtil::prev_in_order;
    using BalanceUtil::is_b_tree;

    // 成员变量
    using BalanceUtil::m_;
    using BalanceUtil::m_half_;
    using BalanceUtil::hight_;

public:
    BalanceTree(size_t m/*B树的阶*/)
        : BalanceTreeBase(m > 2 ? m : 3),
          BalanceUtil(m > 2 ? m : 3) // 最小阶为3
    {
    }

    ~BalanceTree()
    {
        destory(_m_impl._root);
        if (insert_array_ != nullptr)
            free_array(insert_array_);
        if (remove_array_ != nullptr)
            free_array(remove_array_);
    }

    bool insert(const T& val)
    {
        // 一次提前申请m_个，后面递增按照2的倍数扩展
        if (insert_array_ == nullptr)
        {
            insert_array_ = buy_array(m_);
        }
        else
        {
            int32_t len = *(insert_array_ - 1);
            if (len <= hight_ + 1)
            {
                free_array(insert_array_);
                insert_array_ = buy_array(len << 1);
            }
        }

        bool res = insert(&_m_impl._root, val, hight_);
        if (!res)
            return false;

        ele_size_ ++;
        return true;
    }

    bool remove(const T& val)
    {
        // 一次提前申请m_个，后面递增按照2的倍数扩展
        if (remove_array_ == nullptr)
        {
            // 一开始就需要将长度设置足够
            remove_array_ = buy_array(hight_ + 1 > m_ ? hight_  + 1 : m_);
        }
        else
        {
            int32_t len = *(remove_array_ - 1);
            if (len <= hight_ + 1)
            {
                free_array(remove_array_);
                remove_array_ = buy_array(len << 2);
            }
        }

        return remove(&_m_impl._root, val, hight_);
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

    void print_tree()
    {
        draw_tree<Node>(_m_impl._root, hight_, m_);
    }

private:
    // 自底向上的非递归插入算法，内容比较抽象，过程直接仿照递归的方式，阅读代码建议先看递归版本
    bool insert(Node** p_ptr, const T& val, int32_t hight)
    {
        Node* ptr = *p_ptr;
        if (ptr == nullptr)
        {
            ptr = buy_node();
            ptr->array_[ptr->size_++].data_ = val;
            *p_ptr = ptr;
            hight_ ++;

            return true;    
        }

        bool ins_res = false;
        Result f_res{nullptr, 0, false};

        // 专门用来存方Node的下层节点的索引index信息
        int32_t* index_array = insert_array_;
        while(hight >= 0)
        {
            ptr = f_res.ptr_ != nullptr ? f_res.ptr_->next_ : ptr;
            if (hight > 0)
            {
                f_res = find_next(ptr, val);
                if (f_res.tag_)
                    return ins_res;
            }
            else // hight == 0
            {
                f_res = find_val(ptr, val);
                if (f_res.tag_)
                    return ins_res;  // 数据存在
            }
            index_array[hight] = f_res.i_;
            hight --;
        }

        Node* new_ptr = nullptr;  // 本层新分裂的Node指针
        Node* old_ptr = nullptr;  // 下层分裂的Node指针
        Node* next_node = nullptr; // ptr指针的影子指针，赋值于ptr变动之前
        Node* orig_ptr = ptr;     // 记录原始的ptr的值
        int32_t index = -1;
        while(++hight <= hight_)
        {
            old_ptr = new_ptr;
            index = index_array[hight];

            ptr = orig_ptr;   // 复用ptr
            if (hight == 0)
            {
                if (ptr->size_ == m_-1)
                {
                    auto pair = split(ptr, index);
                    new_ptr = pair.first;
                    if (pair.second)
                    {
                        ptr = new_ptr;
                    }
                }

                // 必须是前置++, 因为要拷贝ptr->size_后面的next_指针域或者未来可能的Item域
                for(int i = ++ptr->size_; i > index; i--)
                {
                    ptr->array_[i] = ptr->array_[i - 1];
                }
                ptr->array_[index].data_ = val;

            }
            else
            {
                if (new_ptr == nullptr) // 没有继续发生分裂的现象，可以退出
                {
                    break;
                }

                Pair<Node*, bool> pair(nullptr, false);  // 区分节点是否分裂过
                if (ptr->size_ == m_-1)
                {
                    pair = split(ptr, index);
                    new_ptr = pair.first;
                    if (pair.second)
                    {
                        ptr = new_ptr;
                    }
                }
                else
                {
                    new_ptr = nullptr; // 向上过程中如果分裂停止，则将new_ptr置空
                }

                assert(next_node != nullptr);
                /*
                    index为 m_half_-1是分裂节点时特殊情况, 拼接需要依赖ptr和new_ptr两个对象
                    如果是其他情况，则需要先将index的位置腾挪出来，再做赋值处理
                */
                if (index == m_half_ - 1 && pair.first != nullptr && pair.first != ptr)
                {
                    ptr->size_ ++;

                    no_leaf_node_add_ele(ptr, index, next_node);
                    new_ptr->array_[0].next_ = old_ptr;
                    old_ptr->parent_ = new_ptr;
                }
                else 
                {
                    if (ptr->size_ > 0)
                    {
                        // 必须是前置++, 因为要拷贝ptr->size_后面的next_指针
                        for(int i = ++ptr->size_; i > index; i--)
                        {
                            ptr->array_[i] = ptr->array_[i - 1];
                        }

                        no_leaf_node_add_ele(ptr, index, next_node);
                        ptr->array_[index].next_ = ptr->array_[index + 1].next_;
                        ptr->array_[index + 1].next_ = old_ptr;
                    }
                    else // ptr->size_ == 0
                    {
                        ptr->size_ ++;
                        assert(index == 0);
                        //split中已经将尾部的next_node指针拷贝到 ret_ptr中
                        assert(ptr->array_[0].next_ == next_node);

                        no_leaf_node_add_ele(ptr, 0, next_node);
                        ptr->array_[ptr->size_].next_ = old_ptr;
                    }
                    old_ptr->parent_ = ptr;
                }
            }
            next_node = orig_ptr;
            orig_ptr = orig_ptr->parent_;
            ins_res = true;
        }

        if (new_ptr != nullptr)
        {
            Node* new_root = buy_node();
            new_root->array_[new_root->size_].data_ = get_last_data_in_node(_m_impl._root);
            get_last_data_in_node(_m_impl._root) = T();
            _m_impl._root->size_--;
            new_root->array_[new_root->size_++].next_ = _m_impl._root;
            _m_impl._root->parent_ = new_root;
            new_root->array_[new_root->size_].next_ = new_ptr;
            new_ptr->parent_ = new_root;
            _m_impl._root = new_root;
            hight_ ++;
        }

        return ins_res;
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
                if (n_ptr->array_[i - m_half_].next_ != nullptr)
                {
                    n_ptr->array_[i - m_half_].next_->parent_ = n_ptr;
                }
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
                if (n_ptr->array_[i - m_half_ + 1].next_ != nullptr)
                {
                    n_ptr->array_[i - m_half_ + 1].next_->parent_ = n_ptr;
                }
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
                if (n_ptr->array_[i - m_half_ + 1].next_ != nullptr)
                {
                    n_ptr->array_[i - m_half_ + 1].next_->parent_ = n_ptr;
                }
                ptr->array_[i].data_ = T();
                ptr->array_[i].next_ = nullptr;
            }
            n_ptr->size_ = ptr->size_ - m_half_ + 1;
        }

        ptr->size_ = m_ - 1 - n_ptr->size_;
        n_ptr->parent_ = ptr->parent_;

        return Pair<Node*, bool>{n_ptr, shift_node};
    }

    // 自底向上的非递归删除函数, 算法逻辑和对应的递归函数逻辑一致，如果阅读代码，可以先参考递归版本
    bool remove(Node** p_ptr, const T& val, int32_t hight)
    {
        Node* ptr = *p_ptr;
        if (ptr == nullptr) return false;

        bool rm_res = false;
        Result f_res{nullptr, 0, false};

        // Note:
        // 表示 ptr 在上层 pptr中的位置索引，和insert函数中的函数以有差别
        // 根节点的 pptr索引不会用到，所以直接赋值为0
        int index = 0;
        // 专门用来存放每一层在父节点中的索引信息
        int32_t* index_array = remove_array_;
        while(hight >= 0)
        {
            ptr = f_res.ptr_ != nullptr ? f_res.ptr_->next_ : ptr;
            index_array[hight] = index;
            if (hight > 0)
            {
                f_res = find_next(ptr, val);
                if (f_res.tag_)
                {
                    /*
                        在非叶子节点发现待删除的元素的处理流程中, 将不需要向下查询操作，因为ptr->size_
                        就是未来每一层非叶子节点的索引值，这是获取小于val的最大值的方法，该值一定出现
                        在叶子节点中，所以可以直接将每一次遍历的结果提前填充到index_array。
                        还有一个需要关注的点，index_array的值表示对应层数索引的Node的下一层Node在当前
                        Node中的索引值，所以需要先将hight --,再将索引值保存到index_array数组中。
                    */
                    ptr = f_res.ptr_->next_;
                    hight --;
                    index_array[hight] = f_res.i_;

                    int32_t ptr_size = 0;
                    while(hight > 0)
                    {
                        ptr_size = ptr->size_;
                        ptr = ptr->array_[ptr_size].next_;
                        hight --;
                        index_array[hight] = ptr_size;
                    }
                    hight --;
                    f_res.ptr_->data_ = ptr->array_[ptr->size_ -1].data_;  // 替换
                    index = ptr->size_ - 1; // 填充叶子节点的索引值，后面直接根据该索引值进行删除
                    break;
                }
            }
            else
            {
                f_res = find_val(ptr, val);
                if (!f_res.tag_)
                {
                    return rm_res; // 没有找到直接返回
                }
            }
            index = f_res.i_;
            hight --;
        }
        assert(hight == -1);

        // 删除叶子节点
        remove_ele(ptr, index);
        rm_res = true;

        Node* pptr = nullptr;
        int32_t next_index = -1;  // 记录ptr的下一级节点的索引位置

        while(++hight <= hight_)
        {
            next_index = index_array[hight];
            pptr = ptr->parent_;

            if (pptr == nullptr)
            {
                assert(hight == hight_);
                break;
            }

            if (ptr->size_ < m_half_ - 1)
            {
                if (!lend_ele(pptr, next_index))
                {
                    merge_node(pptr, next_index);
                }
            }

            ptr = ptr->parent_;
        }

        ele_size_ --;
        Node*& root = *p_ptr;
        if (root->size_ == 0)
        {
            // 由于总是将右节点合并到左节点, 所以肯定是array_[0]位置的next_非空
            if (root->array_[0].next_ != nullptr)
            {
                assert(ele_size_ > 0);
                Node* new_root = root->array_[0].next_;
                free_node(root);
                root = new_root;
                root->parent_ = nullptr;
            }
            else
            {
                assert(ele_size_ == 0);
                free_node(root);
                root = nullptr;
            }
            hight_--;
        }

        return rm_res;
    }

    /*
        合并算法的几种情况:
        1. 剩余兄弟节点个数大于等于B树的最小子树个数(m/2(上确界))，根的叶子节点大于等于2：
           直接合并, 额外将父节点中的一个元素放置到叶子节点中
        2. 如果不满足情况1，则需要将父节点中的一个元素和兄弟节点的元素合并起来，如下面删除7和16
           如果父节点缺少元素还需要合并，则处理链向上传递。(m=5)

                    0,1, _, _, _,
            2                                             0,1, _, _, _,
            |       3,4, _, _, _,    删除7:         2
            5                                             3,4,5,6, _,
                    6,7, _, _, _,
        

                        0,1, _, _, _,
                2                                                 0,1, _, _, _,
                |       3,4, _, _, _,                     2
                5                                         |       3,4, _, _, _,
                        6,7, _, _, _,                     5
        8                                 删除16:         |       6,7, _, _, _,
                        9,10, _, _, _,                    8
                11                                        |       9,10, _, _, _,
                |       12,13, _, _, _,                   11
                14                                                12,13,14,15, _,
                        15,16, _, _, _,
    */
    // 合并算法处理原则: 左右节点都可以合并时，先将右节点合并过来
    // pptr层和本层的节点全部处理完再返回, 如果pptr节点变为空，也并不清理内存，留到上层再处理
    void merge_node(Node* pptr, int32_t index)
    {
        if (pptr == nullptr) return;

        int32_t left_index = 0;
        int32_t right_index = 0;
        Dir lost_ele_dir = Dir::Unknown;

        if (index == pptr->size_)
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
        handle_merge_node(pptr, index, left_index, right_index, lost_ele_dir);
    }

    // 总是将右节点合并给左节点，然后释放有节点的内存
    void handle_merge_node(Node* pptr, int32_t index, int32_t left_index, int32_t right_index,
                           Dir lost_ele_dir)
    {
        assert(pptr->size_ >= right_index);
        assert(pptr->size_ > 0);
        
        shift_node(pptr, index, pptr->array_[left_index].next_, pptr->array_[right_index].next_,
                   lost_ele_dir);
        free_node(pptr->array_[right_index].next_);// 删除右边节点
        pptr->array_[right_index].next_ = nullptr;

        // 挪动pptr的节点
        // 当lost_ele_dir == Dir::Right时, index == pptr->size_, 也就是index指向了
        // pptr的尾部next_, 也就不需要挪动pptr的位置，直接清理即可
        if (lost_ele_dir == Dir::Left)
        {
            assert(index < pptr->size_);
            // pptr->array_[i].next_指针已经就位了, 不需要挪动, 只挪动数据部分
            int i = index;
            pptr->array_[i].data_ = pptr->array_[i + 1].data_;
            i++;
            for (; i <= pptr->size_; i++)
            {
                pptr->array_[i] = pptr->array_[i + 1];
            }
        }
        pptr->array_[pptr->size_--].next_ = nullptr;
        pptr->array_[pptr->size_].data_ = T();
    }

    // 将右边节点的内容全部转移到左边的节点, 同时挪动pptr中的一个元素
    void shift_node(Node* pptr, int32_t index, Node* dst_ptr, Node* src_ptr,
                    Dir lost_ele_dir)
    {
        if (lost_ele_dir == Dir::Left)
            assert(dst_ptr->size_ < m_half_ - 1 && src_ptr->size_ == m_half_ - 1);
        else
            assert(dst_ptr->size_ == m_half_ - 1 && src_ptr->size_ < m_half_ - 1);

        if (index == pptr->size_)
        {
            // index在尾部要特殊处理, 选择pptr的index前一个data_位置进行赋值
            dst_ptr->array_[dst_ptr->size_].data_ = pptr->array_[index - 1].data_;
        }
        else
        {
            dst_ptr->array_[dst_ptr->size_].data_ = pptr->array_[index].data_;
        }
        dst_ptr->size_ ++;
        int i = 0;
        for (; i < src_ptr->size_; i++)
        {
            dst_ptr->array_[dst_ptr->size_++] = src_ptr->array_[i];
            if (dst_ptr->array_[dst_ptr->size_ - 1].next_ != nullptr)
            {
                dst_ptr->array_[dst_ptr->size_ - 1].next_->parent_ = dst_ptr;
            }
        }
        // 仅拷贝尾部next_指针，size_不再增加
        // assert(src_ptr->array_[i].next_ != nullptr); // 只生效在hight != 0
        dst_ptr->array_[dst_ptr->size_].next_ = src_ptr->array_[i].next_;
        if (dst_ptr->array_[dst_ptr->size_].next_ != nullptr)
        {
            dst_ptr->array_[dst_ptr->size_].next_->parent_ = dst_ptr;
        }
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
    std::size_t ele_size_{0}; // 总元素数

    int32_t*    insert_array_{nullptr};  // 提前申请插入过程中的索引数组
    int32_t*    remove_array_{nullptr};  // 提前申请删除过程中的索引数组
};


}
}

#endif