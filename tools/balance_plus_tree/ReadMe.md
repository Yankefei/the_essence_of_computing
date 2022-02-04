```sh
B+树的版本介绍：
这里实现的版本为在Robert Sedgewich所提供B树版本基础上的扩展, 并非Mark Allen Weiss版本的B+树，和国内
教科书作者严蔚敏所提供的B+树版本较为接近。

该版本的特点为：
1. B+树的阶由构造函数参数传入。
2. 当非叶子节点内的元素个数为m个时，那么它下级的节点个数也是m个，而不是类似 2-3-4树那样定义的m+1个节点
3. 节点内元素中存储的值为下级各节点中存储值的最小值，而不是最大值。
4. B+树类内部有一个指向最小值所在Node节点的指针，便于遍历。

下面是一棵3阶B+树的示意图：

                                10-63
                            /              \
                  10-21-51                  63-81
            /        |       \             /      \
    -> 10-15 -> 21-37-44 -> 51-59 ->  63-72  ->  81-91-97  ->

```