#ifndef _TOOLS_BALANCE_TREE_V2_DEBUG_H_
#define _TOOLS_BALANCE_TREE_V2_DEBUG_H_

#include <memory>
#include <cassert>

#include "../balance_tree_debug/balance_tree_base_debug.h"
#include "balance_tree_util_base_debug.h"
#include "balance_tree_print_util_debug.h"

#include "pair.hpp"
#include "stack.h"
#include "algorithm.hpp"

namespace tools
{

namespace b_tree_v2
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


/*严蔚敏版B树 非递归版本*/
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
    using BalanceUtil::get_last_data_in_node;
    using BalanceUtil::get_last_ele_from_leaf;
    using BalanceUtil::lend_ele;
    using BalanceUtil::shift_element;
    using BalanceUtil::no_leaf_node_add_ele;
    using BalanceUtil::remove_ele;

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
        bool res = insert(&_m_impl._root, val, hight_);
        if (!res)
            return false;
        
        ele_size_ ++;
        return true;
    }

    bool remove(const T& val)
    {
        return remove(&_m_impl._root, val, hight_);
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
        /*
            insert函数中，该值表示在下级的目标节点在当前Node中的位置
            remove函数中，该值表示当前Node在上级pptr中的位置
        */
        int32_t index = -1;    
    };

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

        Stack<StackInfo> st;
        while (hight >= 0)
        {
            ptr = f_res.ptr_ != nullptr ? f_res.ptr_->next_ : ptr;
            st.push(ptr, 0);
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
                    return ins_res;
            }
            st.top().index = f_res.i_;
            hight --;
        }

        Node* new_ptr = nullptr;  // 本层新分裂的Node指针
        Node* old_ptr = nullptr;  // 下层分裂的Node指针
        Node* next_node = nullptr; // 上一个top()出去堆栈中的ptr指针
        StackInfo s_node;
        while(++hight <= hight_)
        {
            next_node = s_node.ptr;  // 给next_node赋值
            s_node = st.top();
            st.pop();
            old_ptr = new_ptr;
            ptr = s_node.ptr;

            if (hight == 0)
            {
                if (ptr->size_ == m_-1)
                {
                    auto pair = split(ptr, s_node.index);
                    new_ptr = pair.first;
                    if (pair.second)
                    {
                        ptr = new_ptr;
                    }
                }

                // 必须是前置++, 因为要拷贝ptr->size_后面的next_指针域或者未来可能的Item域
                for(int i = ++ptr->size_; i > s_node.index; i--)
                {
                    ptr->array_[i] = ptr->array_[i - 1];
                }
                ptr->array_[s_node.index].data_ = val;
            }
            else
            {
                if (new_ptr == nullptr) // 没有继续发生分裂的现象，可以退出
                {
                    st.clear();
                    break;
                }

                //Node* orig_ptr = ptr;
                Pair<Node*, bool> pair(nullptr, false);  // 区分节点是否分裂过
                if (ptr->size_ == m_-1)
                {
                    pair = split(ptr, s_node.index);
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
                /*
                    这里犯了刻舟求剑的错误，f_res.i_已经过各种修改，但是还直接获取f_res.ptr_的指针
                    需要提前获取, f_res指向的地址可能已经被修改或者变更了
                    // Node* next_node = f_res.ptr_->next_;
                */
                assert(next_node != nullptr);
                /*
                    f_res.i_为 m_half_-1是分裂节点时特殊情况, 拼接需要依赖ptr和new_ptr两个对象
                    如果是其他情况，则需要先将f_res.i_的位置腾挪出来，再做赋值处理
                */
                if (s_node.index == m_half_ - 1 && pair.first != nullptr && pair.first != ptr)
                {
                    ptr->size_ ++;

                    no_leaf_node_add_ele(ptr, s_node.index, next_node);
                    new_ptr->array_[0].next_ = old_ptr;
                }
                else 
                {
                    if (ptr->size_ > 0)
                    {
                        // 必须是前置++, 因为要拷贝ptr->size_后面的next_指针
                        for(int i = ++ptr->size_; i > s_node.index; i--)
                        {
                            ptr->array_[i] = ptr->array_[i - 1];
                        }

                        no_leaf_node_add_ele(ptr, s_node.index, next_node);
                        ptr->array_[s_node.index].next_ = ptr->array_[s_node.index + 1].next_;
                        ptr->array_[s_node.index + 1].next_ = old_ptr;
                    }
                    else // ptr->size_ == 0
                    {
                        ptr->size_ ++;
                        //assert(ptr != orig_ptr);
                        assert(s_node.index == 0);
                        //split中已经将尾部的next_node指针拷贝到 ret_ptr中
                        assert(ptr->array_[0].next_ == next_node);

                        no_leaf_node_add_ele(ptr, 0, next_node);
                        ptr->array_[ptr->size_].next_ = old_ptr;
                    }
                }
            }

            ins_res = true;
        }

        assert(st.empty());
        if (new_ptr != nullptr)
        {
            Node* new_root = buy_node();
            new_root->array_[new_root->size_].data_ = get_last_data_in_node(_m_impl._root);
            get_last_data_in_node(_m_impl._root) = T();
            _m_impl._root->size_--;
            new_root->array_[new_root->size_++].next_ = _m_impl._root;
            new_root->array_[new_root->size_].next_ = new_ptr;
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

    bool remove(Node** p_ptr, const T& val, int32_t hight)
    {
        Node* ptr = *p_ptr;
        if (ptr == nullptr) return false;

        bool rm_res = false;
        Result f_res{nullptr, 0, false};
        T t_val = val;

        Stack<StackInfo> st;
        int index = 0;
        while(hight >= 0)
        {
            ptr = f_res.ptr_ != nullptr ? f_res.ptr_->next_ : ptr;
            st.push(ptr, index);
            if (hight > 0)
            {
                f_res = find_next(ptr, t_val);
                if (f_res.tag_)
                {
                    auto last_ele = get_last_ele_from_leaf(f_res.ptr_->next_, hight - 1);
                    f_res.ptr_->data_ = last_ele->data_; // 替换
                    t_val = last_ele->data_;
                }
            }
            else
            {
                f_res = find_val(ptr, t_val);
                if (!f_res.tag_)
                {
                    return rm_res; // 没有找到直接返回
                }
            }
            index = f_res.i_;
            hight --;
        }

        // 删除叶子节点
        remove_ele(ptr, index);
        rm_res = true;

        Node* pptr = nullptr;
        int32_t next_index = -1;  // 记录ptr的下一级节点的索引位置
        StackInfo s_node;
        while(++hight <= hight_)
        {
            s_node = st.top();
            st.pop();
            if (!st.empty())
                pptr = st.top().ptr;
            else
                pptr = nullptr; // 当到达根节点时，需要将pptr的指针恢复为nullptr, 准备退出
            ptr = s_node.ptr;
            next_index = s_node.index;

            if (pptr == nullptr) break;

            if (ptr->size_ < m_half_ - 1)
            {
                if (!lend_ele(pptr, next_index))
                {
                    merge_node(pptr, next_index);
                }
            }
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
        }
        // 仅拷贝尾部next_指针，size_不再增加
        // assert(src_ptr->array_[i].next_ != nullptr); // 只生效在hight != 0
        dst_ptr->array_[dst_ptr->size_].next_ = src_ptr->array_[i].next_;
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
            else
            {
                if (ptr->size_ < 1)
                    return res;
            }
        }

        do
        {
            int i = 0;
            int j = 0;
            for (; i <= ptr->size_; i++) // 检查第size_+1个指针的指向
            {
                //assert(ptr->array_[i] != nullptr);
                if (hight > 0)
                {
                    assert(ptr->array_[i].next_ != nullptr);

                    int n_size = ptr->array_[i].next_->size_;
                    if (i == ptr->size_)
                    {
                        if (!alg::le(ptr->array_[i - 1].data_, ptr->array_[i].next_->array_[n_size - 1].data_))
                        {
                            break;
                        }
                    }
                    else
                    {
                        // 当前的值比前一位值的下级节点的最大值还要大，保证有序
                        if (!alg::le(ptr->array_[i].next_->array_[n_size - 1].data_, ptr->array_[i].data_))
                        {
                            break;
                        }
                    }
                }
                if (i > 0 && i != ptr->size_)
                {
                    if (!alg::gt(ptr->array_[i].data_, ptr->array_[j].data_))
                        break;
                }
                j = i; // j落后i一位

                if (!is_b_tree(ptr->array_[i].next_, hight - 1))
                    break;
            }

            if (i != ptr->size_ + 1)
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