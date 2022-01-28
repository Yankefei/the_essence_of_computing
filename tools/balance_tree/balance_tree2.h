#ifndef _TOOLS_BALANCE_TREE2_H_
#define _TOOLS_BALANCE_TREE2_H_

#include <memory>
#include <cassert>

#include "balance_tree_base.h"

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
    int          size_{0};          // 数组包含的元素个数
    _BNode*      parent_{nullptr};  // 指向上级父节点的指针
    _Entry<T>**  array_{nullptr};      // 数据内存后置
};

/*B树*/
/*
    B树的版本为：所有的数据头存储在树叶上，也就是非叶子节点的索引值均可以在叶子节点中找到, B树的阶在
    构造时作为参数传入
    这里实现的版本为Robert Sedgewich所提供的版本, 并非Mark Allen Weiss和国内教科书作者如严蔚敏等
    所提供的版本。 该版本的特点为：
    1. 所有的数据均存储在叶子节点上
    2. 当节点内的元素个数为m个时，那么它下级的节点个数也是m个，而不是类似 2-3-4树那样定义的m+1个节点，
       所以这是一种更接近B+树定义的版本。
    3. 节点内元素中存储的值为下级各节点中存储值的最小值，而不是最大值。

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
        template <typename T2> class BEntry = _Entry>
class BalanceTree : protected BalanceTree_Base<T, Alloc, BNode, BEntry>
{
    typedef BalanceTree_Base<T, Alloc, BNode, BEntry> BalanceTreeBase;
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
        Result(Node* ptr, uint32_t i, bool tag): ptr_(ptr), i_(i), tag_(tag)
        {}
        Node*     ptr_{nullptr};    // 指向找到的节点
        uint32_t  i_{0};            // 在pt_节点中的关键字序号, 待插入或者访问
                                    // 若tag_为false, 则可以在i_位置的前后插入
        bool      tag_{false};      // 查找的结果，true/false  
    };

public:
    BalanceTree(size_t m/*B树的阶*/) : BalanceTreeBase(m)
    {
        _m_impl._root = buy_node();
    }

    ~BalanceTree()
    {
        destory(_m_impl._root);
    }

    bool insert(const T& val)
    {
        bool res = insert(_m_impl._root, val, hight_);
        return res;
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
    bool insert(Node* ptr, const T& val, size_t hight)
    {

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

    Result find_val(Node* ptr, const T& val)   // pass
    {
        Result res{ptr, 0, false};
        if (ptr == nullptr || ptr->size_ == 0) return res;
        // 二分查找
        int begin_index = 0;
        int end_index = ptr->size_ - 1;
        if (alg::le(val, ptr->array_[begin_index]->data_))
        {
            res.i_ = begin_index;
            return res;
        }
        if (alg::le(ptr->array_[end_index]->data_, val))
        {
            res.i_ = end_index;
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
        res.i_ = end_index;  // ebd_index < begin_index  未找到
        return res;
    }

    Node* find_next(Node* ptr, const T& val) // pass
    {
        // 二分查找
        bool res = false;
        int begin_index = 0;
        int end_index = ptr->size_ -1;

        assert(ptr->size_ != 0);

        // 提前处理在begin end边界外的情况
        if (alg::le(val, ptr->array_[begin_index]->data_))
            return ptr->array_[begin_index]->next_;
        if (alg::gt(val, ptr->array_[end_index]->data_))
            return ptr->array_[end_index]->next_;

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
        return ptr->array_[end_index]->next_;
    }

    // 判断是否为一棵符合规范的B树

    
    /*
        1. 有序
        2. 节点元素数 m/2 <= x <= m, 根除外（2 <= x <= m）
        3. 所有叶子节点等高
        4. 非叶子节点的元素均需要出现在叶子节点中
    */
    bool is_b_tree(Node* ptr)
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
            destory(ptr->array_[i].next_);
        }
        free_node(ptr);
    }

private:
    std::size_t hight_{0};  // 树高, 单节点的B树默认树高为0
    std::size_t ele_size_{0}; // 总元素数
};

}
}

#endif