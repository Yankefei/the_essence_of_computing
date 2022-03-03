# 简介

本目录中包含了三部分内容：

- 第一部分是一个实验性质的STL库，包含了常见的所有容器以及对应的算法，也额外添加了一些拓展容器。

- 第二部分是一个lock-free无锁队列的容器库，包含了个人实现的SPSCQueue和MPMCQueue两个容器，以及对<br>
一些开源代码的分析和整理，也包含了一些个人对无锁算法底层知识的整理。

- 第三部分是针对用于数据库索引所实现的的B树，B+树容器库。

以上所有的代码和算法均通过了正确性测试和一部分性能测试，作为笔者日常的资料整理和实践使用。

## 一. STL库
### 主要部分
- algorithm.hpp : <br>包含了常用的一些排序，字符查找等算法的实现
- binary_tree.h, binary_tree2.h, binary2_iterator.h:<br>  简单的二叉树容器类以及迭代器类
- binary_tree_util.h: <br>包含了二叉树的常见算法和工具函数
- binary_tree_print_util.h <br>按照从上到下的方式打印一棵二叉树的结构, 用于调试AVL树和RB树
- binary_thread_tree.h binary_thread_tree2.h<br> 中序线索二叉树的容器类
- iterator_base.h <br>容器迭代器的基类（待完善）
- hash_map.hpp, hash_func.h<br> 
基于*开放定址法（平方探测)* 的哈希冲突处理方案, 区别于unordered_map的*链地址法*，目的在于提高查找的效率<br>
但由于使用了数组，在插入和删除中需要额外处理数组的扩容。实际性能测试中，查找效率优于 unordered_map<br>
插入，删除过程的时延较大，为了更快的遍历操作，将数据的下标保存在排序二叉树中。适合**数据变动较少的查找<br>密集型场景**。
- list.hpp<br> 一个双向循环链表的容器，基本覆盖常用方法
- binary_sort_tree 目录<br>
排序二叉树容器，分别实现了带有父指针和不带父指针的两个版本，每个版本实现了递归和非递归的插入以及删除操作,<br>
以及正反的遍历操作。
- binary_avl_tree 目录<br>
平衡二叉树容器，实现了三个版本：携带树高度信息且无parent指针的版本，携带平衡因子且无parent指针的版本，以及<br>
携带平衡因子且有parent指针的版本，详细信息请看该目录的[ReadMe.md](./binary_avl_tree/ReadMe.md)文件。
- binary_rb_tree 目录<br>
红黑二叉树容器，实现了两个版本：不携带parent指针的版本以及携带parent指针的版本. 详细信息请看该目录的[ReadMe.md](./binary_rb_tree/ReadMe.md)<br>文件。
- map.hpp, pair.hpp<br> 
分别使用AVL树和红黑树作为底层数据结构的map容器。
- set.hpp<br>
分别使用AVL树和红黑树作为底层数据结构的set容器。
- vector.hpp<br>
基本覆盖常用方法的vector容器。
- priority_queue 目录<br>
实现了两种优先级队列的容器，第一种是使用传统堆算法的容器适配器，底层使用vector容器。第二种实现为一个索引堆<br>
内部无需直接引用待排序的数据内容，只需提供数据的索引值以及数据之间的比较运算符，就可以对数据进行排序。且支<br>
持删除堆中任意位置的索引。
- queue.h, ring_queue.h<br>
实现了基于链表的普通队列以及基于数组的环形队列。
- stack.h<br>
实现了基于数组的栈容器。
-string.hpp
基本覆盖常用方法的string容器，内部内存布局采用vector数组的布局方式。

### 其他部分
- rand.h<br>
封装了c++11随机库的简单随机数生成器。
- timer_guard.h<br>
一个基于RAII机制的计时器类，精度为纳秒。
- spin_lock.h<br>
一个封装c++11原子模板的简单自旋锁。
- general.h<br>
一些常用的宏函数定义。
- stream.h, stream.cpp<br>
封装了输出流，可支持同时向文件和cout写入。
- util.h, util.cpp<br>
简单工具函数。

## 二. lock-free库

- 见[ReadMe.md](./lock_free_container/ReadMe.md)文件。<br><br>

## 三. B树，B+树容器
### B树容器版本一
- balance_tree 目录：<br>见[ReadMe.md](./balance_tree/ReadMe.md)文件。
- balance_tree_debug 目录: <br>见[ReadMe.md](./balance_tree_debug/ReadMe.md)文件。
### B树容器版本二
- balance_tree_v2_debug 目录：<br>见[ReadMe.md](./balance_tree_v2_debug/ReadMe.md)文件。
### B+树容器
- balance_plus_tree 目录：<br>见[ReadMe.md](./balance_plus_tree/ReadMe.md)文件。
- balance_plus_tree_debug 目录： <br>见[ReadMe.md](./balance_plus_tree_debug/ReadMe.md)文件。

## 四. 测试
- 正确性测试代码目录：**test**, 无锁容器的正确性测试代码目录：**test_lock_free**。
- 性能测试代码目录：**test_performance**, 目前仅提供无锁容器和Map, HashMap的对比验证。


 --  *by Yankefei <yankefei1114@163.com>*