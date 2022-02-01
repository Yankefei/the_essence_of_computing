#ifndef _TOOLS_BALANCE_TREE_DEBUG_H_
#define _TOOLS_BALANCE_TREE_DEBUG_H_

#include <memory>
#include <cassert>

#include "balance_tree_base_debug.h"
#include "balance_tree_print_util_debug.h"

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
    //_Entry<T>**  array_{nullptr};          // 指针数组 调试不容易,等程序稳定后再更换数据结构
    _Entry<T>*     array_{nullptr};
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
    // using BalanceTreeBase::buy_entry;
    // using BalanceTreeBase::free_entry;

    using BalanceUtil::lend_ele;
    using BalanceUtil::shift_element;
    using BalanceUtil::shift_node;
    using BalanceUtil::remove_ele;
    using BalanceUtil::find_val;
    using BalanceUtil::find_next;

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
        bool res = remove(_m_impl._root, val, hight_);
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

    void print_tree1()
    {
        draw_tree1<Node>(_m_impl._root, hight_, m_);
    }

    void print_tree()
    {
        draw_tree<Node>(_m_impl._root, hight_, m_);
    }

private:
    // 堆栈中的信息
    struct StackInfo
    {
        StackInfo() = default;
        StackInfo(Node* _ptr, int32_t _i) : ptr(_ptr), index(_i) {}
        Node*   ptr = nullptr; //指向当前层Node的指针
        int32_t index = 0;     //目标在当前Node中的位置
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
            ptr->array_[ptr->size_++].data_ = val;
            *p_ptr = ptr;
            hight_ ++;
            return true;
        }

        bool ins_res = false;
        Result f_res{nullptr, 0, false};
        // 只有比B树最小值还小时，才会进入该逻辑内
        bool change_min_ele = alg::le(val, ptr->array_[0].data_) ? true : false;

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
                        ptr->array_[0].data_ = val;
                    }
                    // st.clear();
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
                ptr->array_[s_node.index].data_ = old_ptr->array_[0].data_;
                ptr->array_[s_node.index].next_ = old_ptr;
                // 当下级新增了一个B树最小值时，如果新增了Node, 那么该Node将会插入到
                // 当前层的1号索引位，所以需要将s_node.index + 1，0号索引位还是指向了
                // 之前已有的节点，指向是不需要修改的。但是内部的值在这种情况下还是需要
                // 替换.
                if (change_min_ele)
                {
                    ptr->array_[0].data_ = val;
                }
            }
            else
            {
                ptr->array_[s_node.index].data_ = val;
                ins_res = true;
            }
        }
        assert(st.empty());

        if (new_ptr != nullptr)
        {
            Node* new_root = buy_node();
            Node* old_root = *p_ptr;
            new_root->array_[new_root->size_].data_ = old_root->array_[0].data_;
            new_root->array_[new_root->size_++].next_ = old_root;
            new_root->array_[new_root->size_].data_ = new_ptr->array_[0].data_;
            new_root->array_[new_root->size_++].next_ = new_ptr;
            *p_ptr = new_root;
            hight_ ++;
        }

        return ins_res;
    }

    // 自底向上
    bool remove(Node* ptr, const T& val, int32_t hight)
    {
        if (ptr == nullptr) return false;

        if (hight > 0)
        {

        }
        else if (hight == 0)
        {

        }
        else
        {

        }
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
        }
        if (m_odd)
        {
            n_ptr->array_[i] = ptr->array_[m_half_ + i];
            ptr->array_[m_half_ + i].next_ = nullptr;
            ptr->array_[m_half_ + i].data_ = T();
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