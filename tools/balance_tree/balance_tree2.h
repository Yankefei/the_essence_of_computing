#ifndef _TOOLS_BALANCE_TREE2_H_
#define _TOOLS_BALANCE_TREE2_H_

#include <memory>
#include <cassert>

#include "balance_tree_base.h"
#include "balance_tree_util_base2.h"
#include "balance_tree_print_util.h"

#include "pair.hpp"
#include "algorithm.hpp"

namespace tools
{

namespace b_tree2
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
    _BNode*      parent_{nullptr};         // 指向上级父节点的指针
    _Entry<T>**  array_{nullptr};          // 指针数组
};

/*B树 增加父指针的非递归版本*/
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
    typedef typename BalanceUtil::IterResult  IterResult;

public:
    using BalanceTreeBase::buy_node;
    using BalanceTreeBase::free_node;
    using BalanceTreeBase::_m_impl;
    using BalanceTreeBase::buy_array;
    using BalanceTreeBase::free_array;
    using BalanceTreeBase::buy_entry;
    using BalanceTreeBase::free_entry;
    using BalanceTreeBase::alloc_balance;
    using BalanceTreeBase::check_base_same;
    using BalanceTreeBase::shift_alloc_size;

    using BalanceUtil::lend_ele;
    using BalanceUtil::shift_element;
    using BalanceUtil::shift_node;
    using BalanceUtil::find_val;
    using BalanceUtil::find_next;
    using BalanceUtil::find;

    // 迭代遍历函数
    using BalanceUtil::first;
    using BalanceUtil::next;
    using BalanceUtil::last;
    using BalanceUtil::prev;
    using BalanceUtil::in_order;
    using BalanceUtil::prev_in_order;
    using BalanceUtil::is_b_tree;

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
        if (insert_array_ != nullptr)
            free_array(insert_array_);
        if (remove_array_ != nullptr)
            free_array(remove_array_);
    }

    BalanceTree(const BalanceTree& rhs)
        : BalanceTreeBase(rhs),
          BalanceUtil(rhs)
    {
        hight_ = rhs.hight_;
        ele_size_ = rhs.ele_size_;
        if (rhs.insert_array_ != nullptr)
        {
            insert_array_ = buy_array(*(rhs.insert_array_ - 1));
        }
        if (rhs.remove_array_ != nullptr)
        {
            remove_array_ = buy_array(*(rhs.remove_array_ - 1));
        }
        _m_impl._root = copy_data(rhs._m_impl._root);
    }

    BalanceTree& operator=(const BalanceTree& rhs)
    {
        assert(rhs.m_ == m_);
        if (this != &rhs)
        {
            destory(_m_impl._root);
            //BalanceTreeBase::copy_status(rhs);
            //BalanceUtil::copy(rhs);
            hight_ = rhs.hight_;
            ele_size_ = rhs.ele_size_;
            if (rhs.insert_array_ != nullptr)
            {
                int32_t insert_array_len = *(rhs.insert_array_ - 1);
                if (insert_array_ != nullptr)
                    free_array(insert_array_);
                insert_array_ = buy_array(insert_array_len);
            }
            if (rhs.remove_array_ != nullptr)
            {
                int32_t remove_array_len = *(rhs.remove_array_ - 1);
                if (remove_array_ != nullptr)
                    free_array(remove_array_);
                remove_array_ = buy_array(remove_array_len);
            }

            _m_impl._root = copy_data(rhs._m_impl._root);
        }
        return *this;
    }

    BalanceTree(BalanceTree&& rhs)   noexcept
        : BalanceTreeBase(rhs),
          BalanceUtil(rhs)
    {
        move_tree(std::forward<BalanceTree>(rhs));
    }

    BalanceTree& operator=(BalanceTree&& rhs)   noexcept
    {
        assert(rhs.m_ == m_);
        if (this != &rhs)
        {
            destory(_m_impl._root);
            //BalanceTreeBase::copy_status(rhs);
            //BalanceUtil::copy(rhs);

            if (insert_array_ != nullptr)
                free_array(insert_array_);
            if (remove_array_ != nullptr)
                free_array(remove_array_);

            move_tree(std::forward<BalanceTree>(rhs));
        }
        return *this;
    }

    // 自底向上 非递归 去掉对堆栈的依赖
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
        if (res)
            ele_size_ ++;
        
        return res;
    }

    // 自底向上  非递归  去掉对堆栈的依赖
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

        bool res = remove(&_m_impl._root, val, hight_);
        if (res)
            ele_size_ --;

        return res;
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

    bool memory_alloc_balance()
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

    bool is_same(const BalanceTree& rhs)
    {
        return check_same(_m_impl._root, rhs._m_impl._root) &&
                m_ == rhs.m_ &&
                m_half_ == rhs.m_half_ &&
                hight_ == rhs.hight_ &&
                ele_size_ == rhs.ele_size_ &&
                check_base_same(rhs) &&
                check_same2(_m_impl._root, rhs._m_impl._root);
    }

public:
    // 复制一棵B树，返回新的ptr指针, 同时维护 parent_ 指针
    // 使用中序遍历的方式
    Node* copy_data(Node* ptr)
    {
        if (ptr == nullptr) return ptr;

        // 先获取B树的最小节点指针
        Node* left = ptr;
        while(left->array_[0]->next_)
            left = left->array_[0]->next_;

        Node* new_ptr = nullptr;
        Node* new_ptr_next = nullptr;
        // 依次向上遍历，然后递归处理其他部分
        for (; left != ptr->parent_; left = left->parent_)
        {
            new_ptr_next = new_ptr;
            new_ptr = buy_node();
            new_ptr->size_ = left->size_;
            for (int i = 0; i < left->size_; i++)
            {
                Entry* e = left->array_[i];
                Entry*& new_e = new_ptr->array_[i];
                new_e = buy_entry(e->data_);
                if (i == 0)
                {
                    // 处理最左边的情况
                    new_e->next_ = new_ptr_next;
                    if (new_ptr_next != nullptr)
                        new_ptr_next->parent_ = new_ptr;
                }
                else
                {
                    // 处理其他情况
                    new_e->next_ = copy_data(e->next_);
                    if (new_e->next_ != nullptr)
                        new_e->next_->parent_ = new_ptr;
                }
            }
        }

        return new_ptr;
    }

    void move_tree(BalanceTree&& rhs)   noexcept
    {
        hight_ = rhs.hight_;
        ele_size_ = rhs.ele_size_;
        rhs.hight_ = -1;
        rhs.ele_size_ = 0;

        insert_array_ = rhs.insert_array_;
        rhs.insert_array_ = nullptr;

        remove_array_ = rhs.remove_array_;
        rhs.remove_array_ = nullptr;

        _m_impl._root = rhs._m_impl._root;
        rhs._m_impl._root = nullptr;

        shift_alloc_size(rhs);
    }

    bool check_same(Node* ptr, Node* ptr2)
    {
        if (ptr == nullptr && ptr2 == nullptr) return true;

        bool res = false;
        do
        {
            if (ptr == nullptr || ptr2 == nullptr)
                break;

            if (!alg::eq(ptr->size_, ptr2->size_))
                break;

            int i = 0;
            for (; i < ptr->size_ ; i++)
            {
                Entry* e1 = ptr->array_[i];
                Entry* e2 = ptr2->array_[i];
                if (!alg::eq(e1->data_, e2->data_))
                    break;
                if (!check_same(e1->next_, e2->next_))
                    break;
            }

            if (i == ptr->size_) res = true;
        } while (false);
        
        return res;
    }

    // 检查parent_指针是否指向一致
    bool check_same2(Node* root1, Node* root2)
    {
        {
            IterResult begin = first(root1, hight_);
            IterResult begin2 = first(root2, hight_);
            for (; begin.node_ptr != nullptr; begin = next(begin), begin2 = next(begin2))
            {
                if (alg::neq(begin.entry_ptr->data_, begin2.entry_ptr->data_))
                    return false;
            }
            if (begin2.node_ptr != nullptr) return false;
        }

        {
            IterResult end = last(root1, hight_);
            IterResult end2 = last(root2, hight_);
            for (; end.node_ptr != nullptr; end = prev(end), end2 = prev(end2))
            {
                if (alg::neq(end.entry_ptr->data_, end2.entry_ptr->data_))
                    return false;
            }
            if (end2.node_ptr != nullptr) return false;
        }
        return true;
    }

private:
    // 自底向上
    // 算法主体和递归的插入版本过程一致
    bool insert(Node** p_ptr, const T& val, int32_t hight)
    {
        Node* ptr = *p_ptr;
        if (hight == -1)
        {
            assert(ptr == nullptr);
            ptr = buy_node();
            ptr->array_[ptr->size_++] = buy_entry(val);
            *p_ptr = ptr;
            hight_ ++;
            return true;
        }

        bool ins_res = false;
        Result f_res{nullptr, 0, false};
        bool change_min_ele = alg::le(val, ptr->array_[0]->data_) ? true : false;
        // 专门用来存方Node的下层节点的索引index信息
        int32_t* index_array = insert_array_;
        while(hight >= 0)
        {
            ptr = f_res.ptr_ != nullptr ? f_res.ptr_->next_ : ptr;
            if (hight > 0)
            {
                f_res = find_next(ptr, val);
            }
            else // hight == 0
            {
                f_res = find_val(ptr, val);
                if (f_res.tag_)
                {
                    return ins_res;  // 数据存在
                }
            }
            index_array[hight] = f_res.i_;
            hight --;
        }

        Node* new_ptr = nullptr;  // 用于记录新增的节点指针，将来会和原指针作比较
        Node* old_ptr = nullptr;  // 记录之前下一级新增的节点指针
        int32_t index = -1;
        while(++hight <= hight_)
        {
            assert(ptr != nullptr);
            old_ptr = new_ptr;
            index = index_array[hight];
            if (hight == 0)
            {
                if (ptr->size_ == m_)
                {
                    auto pair = split(ptr, index);
                    new_ptr = pair.first;
                    if (pair.second)
                    {
                        ptr = new_ptr;
                    }
                }
            }
            else
            {
                if (new_ptr == nullptr)
                {
                    if (change_min_ele)
                        ptr->array_[0]->data_ = val;
                    ptr = ptr->parent_;
                    continue;
                }
                
                if (ptr->size_ == m_)
                {
                    index += 1;
                    auto pair = split(ptr, index);
                    new_ptr = pair.first;
                    if (pair.second)
                    {
                        ptr = new_ptr;
                    }
                }
                else
                {
                    index ++;   // 非叶子节点的插入位置在原索引位置 +1 的位置
                    new_ptr = nullptr; // 向上过程中如果分裂停止，则将new_ptr置空
                }
            }

            for (int i = ptr->size_ ++; i > index; i--)
            {
                ptr->array_[i] = ptr->array_[i - 1];
            }

            if (hight > 0)
            {
                assert(old_ptr != nullptr);
                ptr->array_[index] = buy_entry(old_ptr->array_[0]->data_);
                ptr->array_[index]->next_ = old_ptr;
                old_ptr->parent_ = ptr;
                // 当下级新增了一个B树最小值时，如果新增了Node, 那么该Node将会插入到
                // 当前层的1号索引位，所以需要将 index + 1，0号索引位还是指向了
                // 之前已有的节点，指向是不需要修改的。但是内部的值在这种情况下还是需要
                // 替换.
                if (change_min_ele)
                {
                    ptr->array_[0]->data_ = val;
                }
            }
            else
            {
                ptr->array_[index] = buy_entry(val);
                ins_res = true;
            }

            ptr = ptr->parent_;
        }

        if (new_ptr != nullptr)
        {
            Node* new_root = buy_node();
            Node* old_root = *p_ptr;
            new_root->array_[new_root->size_] = buy_entry(old_root->array_[0]->data_);
            new_root->array_[new_root->size_++]->next_ = old_root;
            old_root->parent_ = new_root;
            new_root->array_[new_root->size_] = buy_entry(new_ptr->array_[0]->data_);
            new_root->array_[new_root->size_++]->next_ = new_ptr;
            new_ptr->parent_ = new_root;
            *p_ptr = new_root;
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

    // 自底向上
    // 算法主体和递归的删除版本过程一致
    bool remove(Node** p_ptr, const T& val, int32_t hight)
    {
        Node* ptr = *p_ptr;
        if (ptr == nullptr) return false;

        bool rm_res = false;
        Result f_res{nullptr, 0, false};
        int32_t change_min_ele_hight = -1;

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
                if (f_res.tag_ && change_min_ele_hight == -1)
                    change_min_ele_hight = hight; // 至少说明从该层之下的非叶子节点删除时需要更新内容
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

        // 删除叶子节点
        remove_ele(ptr, f_res.i_);
        rm_res = true;

        Node* pptr = nullptr; // 父节点指针
        bool is_combin_node = false; // 是否合并过节点
        int32_t next_index = -1;   // 记录ptr的下一级节点的索引位置
        while(++hight <= hight_)
        {
            if (hight != 0)
                next_index = index_array[hight - 1];
            index = index_array[hight];  // 复用index

            if (hight > 0)
            {
                if (hight <= change_min_ele_hight)
                {
                    // 这里处理的情况是，当ptr的下层节点的节点数 > m_half, 那么删除后
                    // 本层是不需要做任何节点指向层面的修改，但是有可能下层修改了最小节点的值
                    // 如 hight <= change_min_ele_hight时必然发生该情况，所以需要进行
                    // data_值的更换
                    if (ptr->size_ > next_index)
                    {
                        assert(ptr->array_[next_index]->next_ != nullptr);
                        ptr->array_[next_index]->data_
                            = ptr->array_[next_index]->next_->array_[0]->data_;
                    }
                }

                if (!is_combin_node || ptr->size_ >= m_half_)
                {
                    ptr = ptr->parent_;
                    continue;
                }
            }

            // 根节点
            if (ptr->parent_ == nullptr)
                break;

            pptr = ptr->parent_;
            if (ptr->size_ == 0)
            {
                free_node(ptr);
                free_entry(pptr->array_[index]);
                for (int i = index + 1; i < pptr->size_; i++)
                {
                    pptr->array_[i - 1] = pptr->array_[i];
                }
                // pptr->array_[i-1]和pptr->array_[i]的指针完全一致
                // 如果将pptr->array_[i]的指针清理，则完全没有问题
                // 但如果将pptr->array_[i]->next进行清理，则就会影响到
                // pptr->array_[i-1]的数据
                // /*pptr->array_[pptr->size_ - 1]->next_ = nullptr;*/
                pptr->array_[pptr->size_ - 1] = nullptr;
                pptr->size_ --;
                is_combin_node = true;
            }
            else if (ptr->size_ < m_half_)
            {
                // 向左右兄弟节点借一个节点，否则将对下级节点进行合并处理
                if (lend_ele(pptr, index))
                {
                    is_combin_node = false;
                }
                else
                {
                    merge_node(pptr, index);
                    is_combin_node = true;
                }
            }
            else
                is_combin_node = false;

            ptr = pptr;
        }

        // 根节点特殊处理
        Node*& root = *p_ptr;
        while(hight_ > 0 && root->size_ == 1)
        {
            Node* ptr = root->array_[0]->next_;
            free_entry(root->array_[0]);
            root->size_ --;
            free_node(root);
            root = ptr;
            ptr->parent_ = nullptr;
            hight_ --;
        }

        if (root->size_ == 0)
        {
            free_node(root);
            root = nullptr;
            hight_ --;
        }

        return rm_res;
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

    int32_t*    insert_array_{nullptr};  // 提前申请插入过程中的索引数组
    int32_t*    remove_array_{nullptr};  // 提前申请删除过程中的索引数组
};

}
}

#endif