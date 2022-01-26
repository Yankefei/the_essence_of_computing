#ifndef _TOOLS_BALANCE_TREE_H_
#define _TOOLS_BALANCE_TREE_H_

#include <memory>
#include <cassert>

#include "balance_tree_base.h"

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
    int        size_{0};          // 数组包含的元素个数
    _Entry<T>*  array_[0];         // 指针数据内存后置
};

/*B树*/
/*
    B树的版本为：所有的数据头存储在树叶上，也就是非叶子节点的索引值均可以在叶子节点中找到, B树的阶在
    构造时作为参数传入
    这里实现的版本为Robert Sedgewich所提供的版本, 并非Mark Allen Weiss和国内教科书作者如严蔚敏等
    所提供的版本。 该版本的特点为：
    1. 当节点内的元素个数为m个时，那么它下级的节点个数也是m个，而不是类似 2-3-4树那样定义的m+1个节点，
       所以这是一种更接近B+树定义的版本。
    2. 节点内元素中存储的值为下级各节点中存储值的最小值，而不是最大值。

    下面是一棵3阶B树的示意图：

                                  10-63
                             /              \
                        10-21-51              63-81
                    /     |     \             /    \
               10-15  21-37-44   51-59     63-72   81-91-97
*/
template<typename T,
        typename Alloc = std::allocator<T>,
        template <typename T1> class BNode = _BNode,
        template <typename T2> class Entry = _Entry>
class BalanceTree : protected BalanceTree_Base<T, Alloc, BNode, Entry>
{
    typedef BalanceTree_Base<T, Alloc, BNode, Entry> BalanceTreeBase;
    typedef typename BalanceTreeBase::Node Node;
    typedef typename BalanceTreeBase::Entry Entry;
    typedef Node*  Root;
public:
    using BalanceTreeBase::buy_node;
    using BalanceTreeBase::free_node;
    using BalanceTreeBase::_m_impl;
    using BalanceTreeBase::buy_entry;
    using BalanceTreeBase::free_entry;

    /*记录查询的结果*/
    struct Result
    {
        Result(Entry* ptr, int32_t i, bool tag): ptr_(ptr), i_(i), tag_(tag)
        {}
        Entry*    ptr_{nullptr};    // 指向找到的节点
                                    // 在叶子节点中，如果tag_为false, 则为nullptr
                                    // 在非叶子节点中，该ptr_不为空
        int32_t   i_{0};            // 在pt_节点中的关键字序号, 待插入或者访问
                                    // 若tag_为false, 则可以在i_位置进行插入
        bool      tag_{false};      // 查找的结果，true/false  
    };

public:
    BalanceTree(size_t m/*B树的阶*/) : BalanceTreeBase(m), _m(m)
    {
        // _m_impl._root = buy_node();
    }

    ~BalanceTree()
    {
        destory(_m_impl._root);
    }

    bool insert(const T& val)
    {
        Node* ptr = insert(_m_impl._root, val, hight_);

        return true;
    }

    bool remove(const T& val)
    {

    }

    bool search(const T& val)
    {
        return true;
    }

    bool is_b_tree()
    {

    }

    Node* get_root()
    {
        return _m_impl._root;
    }

public:
    // 在递归调用之后检查节点的分裂，程序处理的方向就是自底向上
    Node* insert(Node* ptr, const T& val, int hight)
    {
        Node* n_ptr = nullptr;      // 保存下层递归的返回值
        Node* ret_ptr = ptr;        // 返回的指针
        Result res{nullptr, 0, false};
        bool   change_min_ele = false;
        if (hight > 0)
        {
            res = find_next(ptr, val);
            if (alg::le(val, res.ptr_->array[0]->data_))
            {
                //只有在val比B树最小值还小的时候, 才会进入该逻辑
                change_min_ele = true;
            }

            n_ptr = insert(res.ptr_, val, hight - 1);
            // 一致则说明下层未发生分裂
            if (n_ptr == res.ptr_)
            {
                if (change_min_ele)
                {
                    n_ptr->array[0].data_ = val;
                }
                // 可能下游将最小值更新，所以这里需要再额外判断
                return ret_ptr;
            }
        }
        else if (hight == 0)
        {
            res = find_val(ptr, val);
            if (res.tag_) return ret_ptr;   // 数据存在
        }
        else  // hight < 0
        {
            assert (ptr == nullptr && ptr->size_ == 0);
            ret_ptr = buy_node();
            ptr->array_[ret_ptr->size_++] = buy_entry(val);
            return ret_ptr;
        }

        // 需要进行插入处理
        // 需要先分裂，再插入
        // 这里复用ptr, 赋值为需要插入的节点
        if (ptr->size_ == m_)
        {
            ret_ptr = split(ptr);
            static int32_t m_half = m_ / 2;
            if (res.i_ > m_half)
            {
                res.i_ = res.i_ % m_half;  //插入的新位置
                ptr = ret_ptr;
            }
            else
            {
                // 在原始的节点中更新最小值
                if (change_min_ele)
                {
                    ptr->array[0].data_ = val;
                }
            }
        }

        // ptr->size_ 是需要++的，因为这个时候数量还是之前的数量
        for(int i = ptr->size_ ++; i > res.i_; i--)
        {
            ptr->array_[i] = ptr->array_[i - 1];
        }
        // 非叶子节点插入时, 可能需要更新
        if (hight > 0)
        {
            ptr->array_[res.i_] = buy_entry(n_ptr->array[0]->data_);
            ptr->array_[res.i_]->next_ = n_ptr;
        }
        else
        {
            ptr->array_[res.i_] = buy_entry(val);
        }

        // 返回ret_ptr
        return ret_ptr;
    }

    // 分裂节点, 返回分裂后Node节点指针
    Node* split(Node* ptr)
    {
        assert(ptr->size_ == m_);
        static int32_t m_half = m_ / 2;
        static bool m_odd = m_ % 2 == 0 ? false : true;
        Node* n_ptr = buy_node();
        // m_为奇数时, 在原节点保留元素的少, 迁移的元素较多
        int i = 0;
        for (; i < m_half; i++)
        {
            n_ptr->array_[i] = ptr->array_[m_half + i];
        }
        if (m_odd)
            n_ptr->array_[i] = ptr->array_[m_half + i];

        n_ptr->size_ = m_ - m_half;
        ptr->size_ = m_half;
        return n_ptr;
    }

    Result find(Node* ptr, const T& val, int hight)
    {
        while(hight > 0)
        {
            ptr = find_next(ptr, val);
            hight --;
        }
        return find_val(ptr, val);
    }

    // 返回的Result中, 可能存在位置 i_ >= size_, 表示未来的插入位置可能不需要位置挪动, 或者在新的节点中
    Result find_val(Node* ptr, const T& val)   // pass
    {
        Result res{nullptr, 0, false};
        if (ptr == nullptr || ptr->size_ == 0) return res;
        // 二分查找
        int begin_index = 0;
        int end_index = ptr->size_ - 1;
        if (alg::le(val, ptr->array_[begin_index]->data_))
        {
            return res;
        }
        if (alg::le(ptr->array_[end_index]->data_, val))
        {
            res.i_ = end_index + 1;
            return res;
        }

        int index = 0;
        while(begin_index <= end_index)
        {
            index = (begin_index + end_index) / 2;
            if (alg::eq(val, ptr->array_[index]->data_))
            {
                res.i_ = index;
                res.tag_ = true;
                return true;
            }
            else if (alg::le(val, ptr->array_[index]->data_))
            {
                end_index = index - 1;
            }
            else
            {
                begin_index = index + 1;
            }
        }
        res.i_ = end_index;  // end_index < begin_index  未找到
        return res;
    }

    // 返回的Result中, i_的位置不会超过size_的位置
    Result find_next(Node* ptr, const T& val) // pass
    {
        // 二分查找
        Result res = {nullptr, 0, false};
        int begin_index = 0;
        int end_index = ptr->size_ -1;

        assert(ptr->size_ != 0);

        // 提前处理在begin end边界外的情况
        if (alg::le(val, ptr->array_[begin_index]->data_))
        {
            res.ptr_ = ptr->array_[begin_index];
            return res;
        }
        if (alg::gt(val, ptr->array_[end_index]->data_))
        {
            res.ptr_ = ptr->array_[end_index];
            res.i_ = end_index;
            return res;
        }

        // 只要落在这个区间内，必然会有对应的指针返回
        int index = 0;
        while(begin_index <= end_index)
        {
            index = (begin_index + end_index) / 2;
            if (alg::le(val, ptr->array_[index]->data_))
                end_index = index - 1;
            else if (alg::le(ptr->array_[index]->data_, val))
                begin_index = index + 1;
            else
                return ptr->array_[index]->next_;
        }
        // 如果找不到，那么就取下确界, 也就是end_index的指针部分返回
        // 这里是有意让其退出循环的
        res.ptr_ = ptr->array_[end_index];
        res.i_ = end_index;
        return res;
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
    bool is_b_tree(Node* ptr, int hight_)
    {
        bool res = true;
        for (int i = 0; i < ptr->size_; i ++)
        {

        }
    }

    void destory(Node* ptr)
    {
        if(ptr == nullptr || ptr->size_ == 0) return;

        for(int i = 0; i < ptr->size_; i++)
        {
            destory(ptr->array_[i]->next_);
        }
        free_node(ptr);
    }

private:
    int         m_;          // B树的阶
    int         hight_{-1};  // 树高, 单节点的B树默认树高为0, 空树默认树高为-1
    std::size_t ele_size_{0}; // 总元素数
};

}
}

#endif