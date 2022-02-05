#ifndef _TOOLS_BALANCE_TREE_H_
#define _TOOLS_BALANCE_TREE_H_

#include <memory>
#include <cassert>

#include "balance_tree_base.h"
#include "balance_tree_util_base.h"
#include "balance_tree_print_util.h"

#include "pair.hpp"
#include "stack.h"
#include "algorithm.hpp"

namespace tools
{

namespace b_tree
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
    _Entry<T>**  array_{nullptr};          // 指针数组
};

/*B树 非递归版本*/
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
    using BalanceTreeBase::buy_entry;
    using BalanceTreeBase::free_entry;
    using BalanceTreeBase::alloc_balance;
    using BalanceTreeBase::copy_status;
    using BalanceTreeBase::check_base_same;
    using BalanceTreeBase::shift_alloc_size;

    using BalanceUtil::copy;
    using BalanceUtil::lend_ele;
    using BalanceUtil::shift_element;
    using BalanceUtil::shift_node;
    using BalanceUtil::find_val;
    using BalanceUtil::find_next;
    using BalanceUtil::find;

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
        hight_ = -1;
        ele_size_ = 0;
    }

    BalanceTree(const BalanceTree& rhs)
        : BalanceTreeBase(rhs),
          BalanceUtil(rhs)
    {
        hight_ = rhs.hight_;
        ele_size_ = rhs.ele_size_;
        _m_impl._root = copy_data(rhs._m_impl._root);
    }

    BalanceTree& operator=(const BalanceTree& rhs)
    {
        assert(rhs.m_ == m_);
        if (this != &rhs)
        {
            destory(_m_impl._root);
            Node* root = copy_data(rhs._m_impl._root);

            //BalanceTreeBase::copy_status(rhs);
            //BalanceUtil::copy(rhs);
            hight_ = rhs.hight_;
            ele_size_ = rhs.ele_size_;
            _m_impl._root = root;
        }
        return *this;
    }

    BalanceTree(BalanceTree&& rhs)   noexcept
        : BalanceTreeBase(rhs),
          BalanceUtil(rhs)
    {
        move_tree(std::forward<BalanceTree>(rhs));
    }

    BalanceTree& operator=(BalanceTree&& rhs) noexcept
    {
        assert(rhs.m_ == m_);
        if (this != &rhs)
        {
            destory(_m_impl._root);
            //BalanceTreeBase::copy_status(rhs);
            //BalanceUtil::copy(rhs);
            move_tree(std::forward<BalanceTree>(rhs));
        }
        return *this;
    }

    // 自底向上 非递归
    bool insert(const T& val)
    {
        bool res = insert(&_m_impl._root, val, hight_);
        if (res)
            ele_size_ ++;
        
        return res;
    }

    // 自底向上  非递归
    bool remove(const T& val)
    {
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
                check_base_same(rhs);
    }

public:
    // 复制一棵B树，返回新的ptr指针
    Node* copy_data(Node* ptr)
    {
        if (ptr == nullptr) return ptr;

        Node* new_ptr = buy_node();
        new_ptr->size_ = ptr->size_;
        for (int i = 0; i < ptr->size_; i++)
        {
            Entry* e = ptr->array_[i];
            Entry*& new_e = new_ptr->array_[i];
            new_e = buy_entry(e->data_);
            new_e->next_ = copy_data(e->next_);
        }
        return new_ptr;
    }

    void move_tree(BalanceTree&& rhs) noexcept
    {
        hight_ = rhs.hight_;
        ele_size_ = rhs.ele_size_;
        rhs.hight_ = -1;
        rhs.ele_size_ = 0;

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

private:
    // 堆栈中的信息
    struct StackInfo
    {
        StackInfo() = default;
        StackInfo(Node* _ptr, int32_t _i) : ptr(_ptr), index(_i) {}

        Node*   ptr = nullptr; //指向当前层Node的指针
        /*
            insert函数中，该值表示在下级的目标节点在当前Node中的位置
            remove函数中，该值表示当前Node在上级pptr中的位置
        */
        int32_t index = -1;    
    };

    // 自底向上，在没有parent_指针时，类似AVL树的算法逻辑，必须使用堆栈来记录指针
    // 红黑树那种自顶向下的逻辑就不需要
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
        // 只有比B树最小值还小时，才会进入该逻辑内
        bool change_min_ele = alg::le(val, ptr->array_[0]->data_) ? true : false;

        Stack<StackInfo> st;
        while(hight >= 0)
        {
            ptr = f_res.ptr_ != nullptr ? f_res.ptr_->next_ : ptr;
            st.push(StackInfo{ptr, 0});
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
            st.top().index = f_res.i_;  // 替换下层节点在ptr指针的位置
            hight --;
        }

        Node* new_ptr = nullptr;  // 用于记录新增的节点指针，将来会和原指针作比较
        Node* old_ptr = nullptr;  // 记录之前下一级新增的节点指针
        StackInfo s_node;
        while(++hight <= hight_)
        {
            s_node = st.top();
            st.pop();
            old_ptr = new_ptr;
            ptr = s_node.ptr;
            if (hight == 0)
            {
                if (ptr->size_ == m_)
                {
                    new_ptr = split(ptr);
                    if (s_node.index > m_half_ || s_node.index == m_ - 1)
                    {
                        s_node.index = s_node.index - m_half_;
                        ptr = new_ptr;     // 复用ptr
                    }
                }
            }
            else
            {
                if (new_ptr == nullptr) // 表示下级没有分裂，直接处理返回
                {
                    // 往上的每一层都需要修改
                    if (change_min_ele)
                    {
                        ptr->array_[0]->data_ = val;
                    }
                    continue;
                }

                if (ptr->size_ == m_)
                {
                    new_ptr = split(ptr);
                    // 这里需要包含 == m_half_ 这种情况
                    if (s_node.index >= m_half_ || s_node.index == m_ -1)
                    {
                        s_node.index = s_node.index - m_half_;
                        s_node.index ++;
                        ptr = new_ptr;
                    }
                    else
                    {
                        if (s_node.index != m_half_) // 在分界点不需要挪动索引位置
                            s_node.index ++;
                    }
                }
                else
                {
                    s_node.index ++; // 非叶子节点的插入位置在原索引位置 +1 的位置
                    new_ptr = nullptr; // 向上过程中如果分裂停止，则将new_ptr置空
                }
            }

            for (int i = ptr->size_ ++; i > s_node.index; i--)
            {
                ptr->array_[i] = ptr->array_[i - 1];
            }

            if (hight > 0)
            {
                assert(old_ptr != nullptr);
                ptr->array_[s_node.index] = buy_entry(old_ptr->array_[0]->data_);
                ptr->array_[s_node.index]->next_ = old_ptr;
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
                ptr->array_[s_node.index] = buy_entry(val);
                ins_res = true;
            }
        }
        assert(st.empty());

        if (new_ptr != nullptr)
        {
            Node* new_root = buy_node();
            Node* old_root = *p_ptr;
            new_root->array_[new_root->size_] = buy_entry(old_root->array_[0]->data_);
            new_root->array_[new_root->size_++]->next_ = old_root;
            new_root->array_[new_root->size_]= buy_entry(new_ptr->array_[0]->data_);
            new_root->array_[new_root->size_++]->next_ = new_ptr;
            *p_ptr = new_root;
            hight_ ++;
        }

        return ins_res;
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
            ptr->array_[m_half_ + i] = nullptr;
        }
        if (m_odd)
        {
            n_ptr->array_[i] = ptr->array_[m_half_ + i];
            ptr->array_[m_half_ + i] = nullptr;
        }

        n_ptr->size_ = m_ - m_half_;
        ptr->size_ = m_half_;
        return n_ptr;
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

        Stack<StackInfo> st;
        // Note:
        // 表示 ptr 在上层 pptr中的位置索引，和insert函数中的函数以有差别
        // 根节点的 pptr索引不会用到，所以直接赋值为0
        int index = 0;
        while(hight >= 0)
        {
            ptr = f_res.ptr_ != nullptr ? f_res.ptr_->next_ : ptr;
            st.push(StackInfo{ptr, index});
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

        Node* pptr = nullptr;      // 父指针
        bool is_combin_node = false; // 是否合并过节点
        int32_t next_index = -1;   // 记录ptr的下一级节点的索引位置
        StackInfo s_node;
        while(++hight <= hight_)
        {
            if (hight != 0)
                next_index = s_node.index;
            s_node = st.top();
            st.pop();
            if (!st.empty())
                pptr = st.top().ptr;
            else
                pptr = nullptr; // 当到达根节点时，需要将pptr的指针恢复为nullptr, 准备退出
            ptr = s_node.ptr;

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
                    continue;
                }
            }

            // 根节点
            if (pptr == nullptr)
                break;

            if (ptr->size_ == 0)
            {
                free_node(ptr);
                free_entry(pptr->array_[s_node.index]);
                for (int i = s_node.index + 1; i < pptr->size_; i++)
                {
                    pptr->array_[i - 1] = pptr->array_[i];
                }
                pptr->array_[pptr->size_ - 1] = nullptr;
                pptr->size_ --;
                is_combin_node = true;
            }
            else if (ptr->size_ < m_half_)
            {
                // 向左右兄弟节点借一个节点，否则将对下级节点进行合并处理
                if (lend_ele(pptr, s_node.index))
                {
                    is_combin_node = false;
                }
                else
                {
                    merge_node(pptr, s_node.index);
                    is_combin_node = true;
                }
            }
            else
                is_combin_node = false;
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
                assert(ptr->array_[i] != nullptr);
                if (hight > 0)
                {
                    assert(ptr->array_[i]->next_ != nullptr);
                    if (!alg::eq(ptr->array_[i]->data_, ptr->array_[i]->next_->array_[0]->data_))
                        break;
                    
                    if (i > 0)
                    {
                        int n_size = ptr->array_[j]->next_->size_;
                        // 当前的值比前一位值的下级节点的最大值还要大，保证有序
                        if (!alg::le(ptr->array_[j]->next_->array_[n_size - 1]->data_, ptr->array_[i]->data_))
                        {
                            break;
                        }
                    }
                }
                if (i > 0)
                {
                    if (!alg::gt(ptr->array_[i]->data_, ptr->array_[j]->data_))
                        break;
                }
                j = i; // 让j落后i一位

                if (!is_b_tree(ptr->array_[i]->next_, hight - 1))
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
            destory(ptr->array_[i]->next_);
            free_entry(ptr->array_[i]);
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