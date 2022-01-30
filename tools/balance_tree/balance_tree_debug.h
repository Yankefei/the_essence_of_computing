#ifndef _TOOLS_BALANCE_TREE_DEBUG_H_
#define _TOOLS_BALANCE_TREE_DEBUG_H_

#include <memory>
#include <cassert>

#include "balance_tree_base_debug.h"
#include "balance_tree_print_util_debug.h"

#include "pair.hpp"
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
    // using BalanceTreeBase::buy_entry;
    // using BalanceTreeBase::free_entry;

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

    enum class Dir
    {
        Unknown,
        Left,
        Right
    };

public:
    BalanceTree(size_t m/*B树的阶*/)
        : BalanceTreeBase(m > 1 ? m : 2), m_(m > 1 ? m : 2) // 最小阶为2
    {
        m_half_ = m_ / 2;
    }

    ~BalanceTree()
    {
        destory(_m_impl._root);
    }

    // 自底向上 递归
    bool insert(const T& val)
    {
        bool change_min_ele = false;
        if (_m_impl._root != nullptr && alg::le(val, _m_impl._root->array_[0].data_))
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
                new_root->array_[new_root->size_].data_ = _m_impl._root->array_[0].data_;
                new_root->array_[new_root->size_++].next_ = _m_impl._root;
                new_root->array_[new_root->size_].data_ = res.first->array_[0].data_;
                new_root->array_[new_root->size_++].next_ = res.first;
                _m_impl._root = new_root;
            }
        }
        else
        {
            if (change_min_ele)
                _m_impl._root->array_[0].data_ = val;
        }

        ele_size_ ++;

        return true;
    }

    bool remove(const T& val)
    {
        RemoveRes res{false, false};
        res = remove(_m_impl._root, nullptr, val, 1/*未用*/, hight_);

        return res.first;
    }

    bool search(const T& val)
    {
        return true;
    }

    bool is_b_tree()
    {
        return is_b_tree(_m_impl._root, hight_);
    }

    Node* get_root()
    {
        return _m_impl._root;
    }

    void print_tree1()
    {
        draw_tree1<Node>(_m_impl._root, hight_, m_);
    }

    void print_tree()
    {
        draw_tree<Node>(_m_impl._root, hight_, m_);
    }

public:
    /*
        当叶子节点的元素数 < m/2, 则向两边的兄弟节点借, 如果发现两边兄弟节点的元素数均 < m/2,
        则合并两个叶子节点，将元素值较大的节点的元素挪到元素值较小的节点，然后删除前者的节点内存
        非叶子节点的清理策略同叶子节点, 对根节点进行特殊处理
        自底向上的删除
        在递归调用之后检查节点是否需要继续合并，程序处理的方向就是自底向上
    */
    using RemoveRes = Pair<bool /*is remove ele ?*/, bool/*is combine Node struct?*/>;
    // index 为 ptr在pptr中的位置信息
    RemoveRes remove(Node* ptr, Node* pptr, int32_t index, const T& val, int32_t hight)
    {
        RemoveRes n_res(true, false);
        if (ptr == nullptr) return n_res;
        bool change_min_ele = false;

        Result f_res{nullptr, 0, false};     // 返回查找返回值
        if (hight > 0)
        {
            f_res = find_next(ptr, val);
            if (f_res.tag_ /*&& alg::eq(val, ptr->array_[0].data_)*/)
            {
                change_min_ele = true;
            }

            n_res = remove(f_res.ptr_->next_, ptr, f_res.i_, val, hight - 1);
            if (!n_res.first)  return n_res;  // 直接返回

            if (!n_res.second) return n_res;
            
            // 对最小值进行重新赋值
            if (change_min_ele)
                ptr->array_[f_res.i_].data_
                    = ptr->array_[f_res.i_].next_->array_[0].data_;

            if (ptr->size_ >= m_half_) return n_res;
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

        if (pptr == nullptr)
        {
            return n_res;  // 根节点
        }

        // 先处理节点删除后直接为空的情况, 阶为 2, 3时有这种可能
        if (ptr->size_ == 0)
        {
            // 处理逻辑是，先处理这一层的内容，上面的层数由退出递归时处理
            free_node(ptr);
            pptr->array_[pptr->size_ - 1].next_ = nullptr;
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

    // 先向右兄弟节点借, 再向左兄弟节点借
    // using LendRes = Pair<bool/*lend result*/, int32_t/*first_ele_change_index, 需要更新ptr中元素的位置*/>;
    bool lend_ele(Node* ptr, int32_t debtor_index/*发起借贷的位置*/)
    {
        bool res = false;
        if (ptr == nullptr) return res;

        assert(ptr->size_ >= debtor_index + 1);
        Dir shift_dir = Dir::Unknown;
        Node* left_ptr = nullptr;
        Node* right_ptr = nullptr;
        // int32_t first_ele_change_index = 0;
        do
        {
            if (debtor_index == 0)  // 左端点
            {
                if (/*ptr->size_ == 1 ||*/ ptr->array_[debtor_index + 1].next_->size_ == m_half_)
                {
                    break;
                }
                shift_dir = Dir::Left;
                left_ptr = ptr->array_[debtor_index].next_;
                right_ptr = ptr->array_[debtor_index + 1].next_;
                // first_ele_change_index = debtor_index + 1;
            }
            else if (debtor_index == ptr->size_ - 1)  // 右端点
            {
                if (ptr->array_[debtor_index - 1].next_->size_ == m_half_)
                {
                    break;
                }
                shift_dir = Dir::Right;
                left_ptr = ptr->array_[debtor_index - 1].next_;
                right_ptr = ptr->array_[debtor_index].next_;
                // first_ele_change_index = debtor_index;
            }
            else
            {
                if (ptr->array_[debtor_index + 1].next_->size_ == m_half_)
                {
                    if (ptr->array_[debtor_index - 1].next_->size_ == m_half_)
                    {
                        break;   
                    }
                    else
                    {
                        shift_dir = Dir::Right;
                        left_ptr = ptr->array_[debtor_index -1].next_;
                        right_ptr = ptr->array_[debtor_index].next_;
                        // first_ele_change_index = debtor_index;
                    }
                }
                else
                {
                    shift_dir = Dir::Left;
                    left_ptr = ptr->array_[debtor_index].next_;
                    right_ptr = ptr->array_[debtor_index + 1].next_;
                    // first_ele_change_index = debtor_index + 1;
                }
            }
            shift_element(left_ptr, right_ptr, shift_dir);
            // 更新非叶子节点的数值
            if (shift_dir == Dir::Left)
            {
                ptr->array_[debtor_index + 1].data_ = right_ptr->array_[0].data_;
            }
            else
            {
                ptr->array_[debtor_index].data_ = left_ptr->array_[0].data_;
            }
            res = true;
        }while(false);

        return res;
    }

    // shift_dir 表示转移节点的方向
    void shift_element(Node* left_ptr, Node* right_ptr, Dir shift_dir)
    {
        if (shift_dir == Dir::Left)
        {
            assert(left_ptr->size_ < m_half_ && right_ptr->size_ > m_half_);
            left_ptr->array_[left_ptr->size_++] = right_ptr->array_[0];
            for (int i = 1; i < right_ptr->size_; i++)
            {
                right_ptr->array_[i - 1] = right_ptr->array_[i];
            }
            right_ptr->size_ --;
        }
        else
        {
            assert(left_ptr->size_ > m_half_ && right_ptr->size_ < m_half_);
            for (int i = right_ptr->size_; i > 0; i--)
            {
                right_ptr->array_[i] = right_ptr->array_[i - 1];
            }
            right_ptr->size_ ++;
            right_ptr->array_[0] = left_ptr->array_[left_ptr->size_ - 1];
            left_ptr->array_[left_ptr->size_ - 1].next_ = nullptr;
            left_ptr->size_ --;
        }
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
    void handle_merge_node(Node* pptr, int32_t left_index, int32_t right_index, Dir lost_ele_dir)
    {
        assert(pptr->size_ >= right_index);
        shift_node(pptr->array_[left_index].next_, pptr->array_[right_index].next_, lost_ele_dir);
        free_node(pptr->array_[right_index].next_);
        for (int i = right_index + 1; i < pptr->size_; i++)
        {
            pptr->array_[i - 1] = pptr->array_[i];
        }
        pptr->array_[pptr->size_ - 1].next_ = nullptr;
        pptr->size_ --;
    }

    // 将src_ptr的所有节点转移到 dst_ptr中, src在右，dst在左, lost_ele_dir 表示两者哪一个是删除元素的节点
    void shift_node(Node* dst_ptr, Node* src_ptr, Dir lost_ele_dir)
    {
        if (lost_ele_dir == Dir::Left)
            assert(dst_ptr->size_ < m_half_ && src_ptr->size_ == m_half_);
        else
            assert(dst_ptr->size_ == m_half_ && src_ptr->size_ < m_half_);

        int i = 0;
        for (; i < src_ptr->size_; i++)
        {
            dst_ptr->array_[dst_ptr->size_++] = src_ptr->array_[i];
        }
    }

    // 删除完毕，进行重排
    void remove_ele(Node* ptr, int32_t index)
    {
        assert(ptr->size_ >= index + 1);
        assert(ptr->array_[index].next_ == nullptr);  // 必须将待删除的节点信息全部维护完才能删除
        int i = index + 1;
        for (; i < ptr->size_; i++)
        {
            ptr->array_[i-1] = ptr->array_[i];
        }
        
        if (i != index + 1)
            ptr->array_[i].next_ = nullptr;
        else
            ptr->array_[index].next_ = nullptr;
        ptr->size_ --;
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
            if (alg::le(val, ptr->array_[0].data_))
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
                    n_res.first->array_[0].data_ = val;
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
            ret_ptr->array_[ret_ptr->size_++].data_ = val;
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
                ret_ptr = split(ptr);
                if (f_res.i_ > m_half_ || f_res.i_ == m_ - 1)  // 后面这种场景是处理m_为2的特殊情况
                {
                    f_res.i_ = f_res.i_ - m_half_;  //插入的新位置
                    ptr = ret_ptr;// 重置ptr
                }
            }
        }
        else
        {
            if (ptr->size_ == m_)
            {
                ret_ptr = split(ptr);
                if (f_res.i_ >= m_half_ || f_res.i_ == m_ - 1)  // 后面这种场景是处理m_为2的特殊情况
                {
                    f_res.i_ = f_res.i_ - m_half_;  //插入的新位置
                    f_res.i_ ++;
                    ptr = ret_ptr;// 重置ptr
                }
                else
                {
                    // 如果刚好m_half, 则不需要更新，因为这个位置为空，可以直接插入
                    if (f_res.i_ != m_half_)
                        f_res.i_ ++;
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
            ptr->array_[f_res.i_].data_ = n_res.first->array_[0].data_;
            ptr->array_[f_res.i_].next_ = n_res.first;
            // 在f_res.i_ 指向原始的节点时，判断是否更新最小值
            if (change_min_ele)
            {
                ptr->array_[0].data_ = val;
            }
        }
        else
        {
            ptr->array_[f_res.i_].data_ = val;
        }

        // 应该返回哪一个节点的指针？新增的节点指针或原ptr指针
        n_res.first = ret_ptr;
        return n_res;
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
        // 二分查找大于等于该值的最小区间, 用于寻找将val插入的位置
        int begin_index = 0;
        int end_index = ptr->size_ - 1;
        if (alg::le(val, ptr->array_[begin_index].data_))
        {
            return res;
        }
        // if (alg::le(ptr->array_[end_index].data_, val))
        // {
        //     res.i_ = end_index + 1;
        //     return res;
        // }

        int index = 0;
        while(begin_index <= end_index)
        {
            index = (begin_index + end_index) / 2;
            if (alg::eq(val, ptr->array_[index].data_))
            {
                res.ptr_ = &ptr->array_[index];
                res.i_ = index;
                res.tag_ = true;
                return res;
            }
            else
            {
                if (end_index - begin_index <= 1)
                {
                    break; // 进入了末端，逐个比较
                }
                if (alg::le(val, ptr->array_[index].data_))
                    end_index = index - 1;
                else
                    begin_index = index + 1;
            }
        }

        // 到最后该选择哪个位置进行替换，还需要再判断
        int i = begin_index;
        for (; i <= end_index; i++)
        {
            if (alg::eq(val, ptr->array_[i].data_))
            {
                res.i_ = i;
                res.ptr_ = &ptr->array_[i];
                res.tag_ = true;
                return res;
            }

            if (alg::le(val, ptr->array_[i].data_))
            {
                // 返回第一个大于val的位置
                res.i_ = i;
                res.ptr_ = &ptr->array_[i];
                return res;
            }
        }
        res.i_ = i;
        res.ptr_ = &ptr->array_[i];
        return res;
    }

    // 返回的Result中, i_的位置不会超过size_的位置
    Result find_next(Node* ptr, const T& val) // pass
    {
        // 二分查找小于等于该值的最大区间，用于从区间的下层搜索val
        Result res = {nullptr, 0, false};
        int begin_index = 0;
        int end_index = ptr->size_ -1;

        assert(ptr->size_ != 0);

        // 提前处理在begin边界外的情况
        if (alg::le(val, ptr->array_[begin_index].data_))
        {
            res.ptr_ = &ptr->array_[begin_index];
            return res;
        }
        // if (alg::gt(val, ptr->array_[end_index].data_))
        // {
        //     res.ptr_ = &ptr->array_[end_index];
        //     res.i_ = end_index;
        //     return res;
        // }

        // 只要落在这个区间内，必然会有对应的指针返回
        int index = 0;
        while(begin_index <= end_index)
        {
            index = (begin_index + end_index) / 2;
            if (alg::eq(val, ptr->array_[index].data_))
            {
                res.ptr_ = &ptr->array_[index];
                res.i_ = index;
                res.tag_ = true;
                return res;
            }
            else
            {
                if (end_index - begin_index <= 1)
                {
                    break; // 进入了末端，逐个比较
                }
                if (alg::le(val, ptr->array_[index].data_))
                    end_index = index - 1;
                else
                    begin_index = index + 1;
            }
        }

        int i = end_index;
        for (; i >= begin_index; i--)
        {
            if (alg::eq(val, ptr->array_[i].data_))
            {
                res.ptr_ = &ptr->array_[i];
                res.i_ = i;
                res.tag_ = true;
                return res;
            }

            if (alg::le(ptr->array_[i].data_, val))
            {
                // 返回第一个小于val的位置
                res.i_ = i;
                res.ptr_ = &ptr->array_[res.i_];
                return res;
            }
        }

        res.i_ = i;
        res.ptr_ = &ptr->array_[res.i_];
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
    int         m_;          // B树的阶
    int         m_half_;     // 阶的一半，计算大量使用到
    int         hight_{-1};  // 树高, 单节点的B树默认树高为0, 空树默认树高为-1
    std::size_t ele_size_{0}; // 总元素数
};

}
}

#endif