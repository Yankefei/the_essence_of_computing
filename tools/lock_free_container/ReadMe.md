```sh
documentation中的文档概述：

1. Documentation_memory-barriers.txt (Documentation_memory-barriers_cn.txt)
2. whymb.2010.06.07c.pdf             (whymb.2010.06.07c_cn.docx)
分别来自linux内核的文档以及IBM研究员的论文，均为对内存屏障的专业性分析。在经过机器翻译后对内容重新
进行了梳理和理解。 "_cn"后缀的即是对应的译文。

如果想直接了解具体的要点，可以直接阅读本文件夹的 [summary_of_point.md](./summary_of_point.md)
```

```sh
代码文件介绍：

文件夹中的kfifo.h 和kfifo.cpp是将linux内核（v4.4.293）的代码迁移到了g++的环境，并编译通过。用于
测试和分析。kfifo_old_version文件夹中的是将linux内核（v2.6.24）的kfifo代码拷贝过来，没有做编译
性迁移，仅仅用来分析和注释里面的代码逻辑。因为这个旧版本的kfifo内部很多处内存屏障的使用不够精简，
可以优化和去掉。
通过对比，可以对kfifo的版本演变有更深入的了解，加深；理解对内存屏障用法的使用。

spsc_queue.h 为无锁的单生产者，单消费者的c++队列容器，实现参考kfifo.

```