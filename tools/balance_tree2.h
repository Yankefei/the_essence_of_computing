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
struct Entry
{
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
    int      size_{0};          // 数组包含的元素个数
    _BNode*  parent_{nullptr};  // 指向上级父节点的指针
    Entry<T> array_[0];      // 数据内存后置
};

/*B树*/
/*
    B树的版本为：所有的数据头存储在树叶上，也就是非叶子节点的索引值均可以在叶子节点中找到, B树的阶在构造时作为参数传入
    这里实现的版本为 Robert Sedgewich 所提供的版本, 并非 Mark Allen Weiss 和国内教科书作者如 严蔚敏等
    所提供的版本。 该版本的特点为：
    1. 当节点内的元素个数为m个时，那么它下级的节点个数也是m个，而不是类似 2-3-4树那么定义的 m+1 个节点，
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
        template <typename T1> class BNode = _BNode>
class BalanceTree : protected BalanceTree_Base<T, BNode, Alloc, sizeof(Entry<T>)>
{
    typedef BalanceTree_Base<T, BNode, Alloc, sizeof(Entry<T>)> BalanceTreeBase;
    typedef typename BalanceTreeBase::Node Node;
    typedef Node*  Root;
public:
    using BalanceTreeBase::buy_node;
    using BalanceTreeBase::free_node;
    using BalanceTreeBase::_m_impl;

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

    bool find(Node* ptr, const T& val, size_t hight)
    {
        if (hight == 0)
        {
            return find_val(ptr, val);
        }
        else
        {
            ptr = find_next(ptr, val);
            if (ptr == nullptr) return false;

            return find(ptr, val, hight - 1);
        }
    }

    bool find_val(Node* ptr, const T& val)   // pass
    {
        if (ptr->size_ == 0) return false;
        // 二分查找
        bool res = false;
        int begin_index = 0;
        int end_index = ptr->size_ - 1;
        if (alg::le(val, ptr->array_[begin_index].data_) ||
            alg::le(ptr->array_[end_index].data_, val))
            return res;

        int index = 0;
        while(begin_index <= end_index)
        {
            index = (begin_index + end_index) / 2;
            if (alg::eq(val, ptr->array_[index].data_))
            {
                return true;
            }
            else if (alg::le(val, ptr->array_[index].data_))
            {
                end_index = index - 1;
            }
            else
            {
                begin_index = index + 1;
            }
        }
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
        if (alg::le(val, ptr->array_[begin_index].data_))
            return nullptr;
        if (alg::gt(val, ptr->array_[end_index].data_))
            return ptr->array_[end_index].next_;

        // 只要落在这个区间内，必然会有对应的指针返回
        int index = 0;
        while(begin_index <= end_index)
        {
            index = (begin_index + end_index) / 2;
            if (alg::le(val, ptr->array_[index].data_))
                end_index = index - 1;
            else if (alg::le(ptr->array_[index].data_, val))
                begin_index = index + 1;
            else
                return ptr->array_[index].next_;
        }
        // 如果找不到，那么就取下确界, 也就是end_index的指针部分返回
        // 这里是有意让其退出循环的
        return ptr->array_[end_index].next_;
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