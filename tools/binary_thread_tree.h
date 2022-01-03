#ifndef _TOOLS_BINARY_THREAD_TREE_H_
#define _TOOLS_BINARY_THREAD_TREE_H_

#include <memory>
#include <cassert>

namespace tools
{

enum class PointerTag
{
    LINK = 0,
    THREAD = 1
};

template<typename T = char>
struct _TNode
{
    _TNode() = default;
    _TNode(const T&val) : data_(val) {}

    _TNode* left_tree_{nullptr};
    _TNode* right_tree_{nullptr};
    T data_;
    PointerTag l_tag;
    PointerTag r_tag;
};

/*中序线索二叉树*/
template<typename T, typename Alloc>
class BtTree_Base
{
public:
    typedef typename std::allocator_traits<Alloc>::template
        rebind_alloc<_TNode<T>> _TNode_alloc_type;

    typedef typename std::allocator_traits<_TNode_alloc_type>
        rebind_traits;

    typedef typename rebind_traits::pointer  pointer;

    typedef _TNode<T>*        Node;

public:
    struct Base_Impl : public _TNode_alloc_type
    {
        Base_Impl(): _TNode_alloc_type(), _root(), _end()
        {
        }

        ~Base_Impl() {}

        Base_Impl(Base_Impl& _x) noexcept
        {
            std::swap(_root, _x._root);
            std::swap(_end, _x._end);
        }

        Node     _root{nullptr};
        T        _end;
    };

    BtTree_Base() {}
    ~BtTree_Base() {} // 设置虚函数，Base_Impl 会额外增加8字节内存

public:
    Base_Impl   _m_impl;

    Node buy_node(const T& val)
    {
        pointer p = rebind_traits::allocate(_m_impl, 1);
        rebind_traits::construct(_m_impl, p, val);
        return static_cast<Node>(p);
    }

    void free_node(Node ptr)
    {
        rebind_traits::destroy(_m_impl, ptr);
        rebind_traits::deallocate(_m_impl, ptr, 1);
    }
};

// template<typename TI,
//     template <typename T1> class Alloc1,
//     template <typename T2, typename Alloc2> class Tree>
// class ThreadInIterator;

template<typename T = char, typename Alloc = std::allocator<T>>
class BtTree : protected BtTree_Base<T, Alloc>
{
    // 这种格式需要详细执行具体的模板类型参数，使用方式有限
    // friend class ThreadInIterator<char, std::allocator, BtTree>;

    // 类模板与类模板之前的友元格式
    // https://blog.csdn.net/caroline_wendy/article/details/16916441
    template<typename TI,
        template <typename T1> class Alloc1,
        template <typename T2, typename Alloc2> class Tree>
    friend class ThreadInIterator;

public:
    typedef _TNode<T>*        Node;
    typedef Node              Root;

protected:
    using BtTree_Base<T, Alloc>::buy_node;
    using BtTree_Base<T, Alloc>::free_node;
    using BtTree_Base<T, Alloc>::_m_impl;

public:
    BtTree() {}
    ~BtTree()
    {
        free();
    }

    BtTree(const T& end)
    {
        _m_impl._end = end;
    }

    void set_end_val(const T& end)
    {
        _m_impl._end = end;
    }

    Root get_root() { return _m_impl._root; }

    void create_tree_by_pre(const T* array)
    {
        if (_m_impl._root)
        {
            destory(_m_impl._root);
        }

        _m_impl._root = _create_tree_by_pre(array);
        MakeThreadTree();
    }

    void free()
    {
        destory(_m_impl._root);
    }

    // 线索化的遍历
    void InOrder()
    {
        _ThreadInOrder(_m_impl._root);
        stream << std::endl;
    }

    // 逆序线索化遍历
    void ResInOrder()
    {
        _ResThreadInOrder(_m_impl._root);
        stream << std::endl;
    }

private:
    // void MakeThreadTree()
    // {
    //     // 设置一个额外的header节点
    //     Node ptr = buy_node(T());
    //     ptr->left_tree_ = _m_impl._root;
    //     ptr->l_tag = PointerTag::LINK;
    //     _im_impl._root = ptr;
    //     MakeThread(_m_impl._root, ptr);
    //     ptr->right_tree_ = nullptr;
    //     ptr->r_tag = PointerTag::THREAD;
    // }

    void MakeThreadTree()
    {
        // 不设置虚拟头部节点
        Node ptr = nullptr;
        MakeThread(_m_impl._root, ptr);
        ptr->right_tree_ = nullptr;
        ptr->r_tag = PointerTag::THREAD;
    }

    void MakeThread(Node p, Node& ptr)
    {
        if (p != nullptr)
        {
            MakeThread(p->left_tree_, ptr);
            if (p->left_tree_ == NULL)
            {
                p->left_tree_ = ptr;
                p->l_tag = PointerTag::THREAD;
            }

            // ptr 是由上一层递归传递而来的
            if (ptr != nullptr && ptr->right_tree_ == nullptr)
            {
                ptr->right_tree_ = p;
                ptr->r_tag = PointerTag::THREAD;
            }
            ptr = p;
            MakeThread(p->right_tree_, ptr);
        }
    }

    Node first(Node ptr)
    {
        while(ptr && ptr->l_tag == PointerTag::LINK)
        {
            ptr = ptr->left_tree_;
        }

        return ptr;
    }

    Node last(Node ptr)
    {
        while(ptr && ptr->r_tag == PointerTag::LINK)
        {
            ptr = ptr->right_tree_;
        }

        return ptr;
    }

    Node next(Node ptr)
    {
        if (!ptr) return ptr;

        if(ptr->r_tag == PointerTag::LINK)
        {
            return first(ptr->right_tree_);
        }
        else
        {
            return ptr->right_tree_;
        }
    }

    Node prev(Node ptr)
    {
        if (!ptr) return ptr;

        if(ptr->l_tag == PointerTag::LINK)
        {
            return last(ptr->left_tree_);
        }
        else
        {
            return ptr->left_tree_;
        }
    }

    // 线索化的清理逻辑
    void destory(Node ptr)
    {
        auto it_ptr = first(ptr);
        for(; it_ptr != nullptr; it_ptr = next(it_ptr))
        {
            free_node(it_ptr);
        }
    }

    Node _create_tree_by_pre(const T*& array)
    {
        Node ptr = nullptr;
        if (array != nullptr && *array != _m_impl._end)
        {
            ptr = buy_node(*array);
            ptr->l_tag = PointerTag::LINK;
            ptr->r_tag = PointerTag::LINK;	
            ptr->left_tree_ = _create_tree_by_pre(++array);
            ptr->right_tree_ = _create_tree_by_pre(++array);
        }
        return ptr;
    }

    void _ThreadInOrder(Node ptr)
    {
        for (auto it_ptr = first(ptr); it_ptr != nullptr; it_ptr = next(it_ptr))
        {
            stream << it_ptr->data_ << " ";
        }
    }

    void _ResThreadInOrder(Node ptr)
    {
        for (auto it_ptr = last(ptr); it_ptr != nullptr; it_ptr = prev(it_ptr))
        {
            stream << it_ptr->data_ << " ";
        }
    }
};


// 模板的模板
// 而且如果模板类型参数之间存在依赖，则需要先将每一项与最原始的类型T之间的关系列在前面，如下
// 且同一类的模板类型参数，如T, T1, T2, 都不能重复出现
template<typename T,
        template <typename T1> class Alloc1,
        template <typename T2, typename Alloc2> class Tree>
class ThreadInIterator
{
protected:
    typedef Alloc1<T>                      Alloc;
    typedef typename Tree<T, Alloc>::Node  BtNodePtr;

protected:
    Tree<T, Alloc>&  tree_;
    BtNodePtr ptr_;

public:
    ThreadInIterator(Tree<T, Alloc>& bt): tree_(bt), ptr_(nullptr) {}
    ~ThreadInIterator() {}

    T& operator*() { return ptr_->data_;}
    const T& operator*() const { return ptr_->data;}
    bool IsDone() const { return ptr_ == nullptr;}

    void First()
    {
        ptr_ = tree_.first(tree_.get_root());
    }
    void operator++()
    {
        ptr_ = tree_.next(ptr_);
    }

    void Last()
    {
        ptr_ = tree_.last(tree_.get_root());
    }

    void operator--()
    {
        ptr_ = tree_.prev(ptr_);
    }
};

template<typename T = char>
using BtTInIterator = ThreadInIterator<T, std::allocator, BtTree>;

template<typename T = char,
         template <typename T1> class Alloc1 = std::allocator,
         template<typename T2, typename Alloc2> class Tree = BtTree>
void Print(ThreadInIterator<T, Alloc1, Tree> &it)
{
    it.First();

    while(!it.IsDone())
    {
        stream << *it << " ";
        ++it;
    }
    stream << std::endl;
}


template<typename T = char,
         template <typename T1> class Alloc1 = std::allocator,
         template<typename T2, typename Alloc2> class Tree = BtTree>
void ResPrint(ThreadInIterator<T, Alloc1, Tree> &it)
{
    it.Last();

    while(!it.IsDone())
    {
        stream << *it << " ";
        --it;
    }
    stream << std::endl;
}

}

#endif