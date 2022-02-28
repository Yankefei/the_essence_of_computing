```c++
memory barriers, 内存屏障简介：

由于现代大多数CPU的指令执行序列可能乱序，原因一部分来自多核CPU执行，一部分来自编译期的优化，为了
保证指令顺序执行，内存屏障发挥了很重大的作用。

memory barriers 包含两类
1. 编译器 barrier.  (又称优化barrier)，用于避免编译期优化带来的内存乱序访问的问题
    linux内核实现的barrier() 如下 (x86-64架构(amd64))
    #define barrier() __asm__ __volatile("" ::: "memory") 
    该指令告知编译器，保存在CPU寄存器中，在屏障之前有效的所有内存地址，在屏障之后都将消失。
    
    关于上面问题的另一个方案：linux内核中提供了一个宏ACCESS_ONCE来避免编译期对实例进行指令优化
    #define ACCESS_ONCE(x) (*(volatile typedef(x)*)&(x)) 
    作用仅将变量x转变为volatile的类型

2. cpu memory barrier. 
    虽然CPU会乱序执行指令，但是在单个CPU上，硬件能够保证程序执行时所有的内存访问操作看起来像是按照
    程序代码编写的顺序执行，这个时候memory barrier没有必要使用（不考虑编译期优化的情况下）
    有结论：1. 对于单个CPU指令获取时有序的（通过队列实现）
            2. 对于单个CPU指令执行结果也是有序返回寄存器堆的（通过队列实现）

    但是SMP((Symmetric Multi Processing),对称多处理系统内有许多紧耦合多处理器)就不一样了, 和cache
    的交互过程中会存在乱序执行的情况，所以需要使用cpu memory barrier:(实现于内核)
    1. 通用barrier, 保证读写操作有序的，mb() 和 smp_mb();
    2. 写操作barrier, 仅保证写操作有序的, wmb() 和 smp_wmb();
    3. 读操作barrier，仅保证读操作有序的，rmb() 和 smp_rmb();
    4. read_barrier_depends, 一种特殊形式的读访问屏障，它会考虑读操作之间的依赖性。如果屏障之后
       的读请求，依赖于屏障之前执行的读请求的数据，那么编译期和硬件都不能重排这些请求。

    memory barrier常用场景包括：
    1. 实现同步原语，一个特定应用是内核抢占机制中，避免指令重排。
    2. 实现无锁数据结构
    3. 驱动程序
    在实际的应用程序开发中，开发者可能完全不知道memory barrier就可以开发正确的多线程程序，这主要是
    因为各种同步机制中已经隐含了memory barrier.(但和实际的memory barrier有细微差别)

    所有的cpu memory barrier都隐含了编译器barrier.
    linux内核的代码会根据配置在单cpu使用编译器barrier, 而在SMP上才使用memory barrier. 另外在SMP
    上如果使用共享内存，也必须使用memory barrier而不是编译期barrier. 当然，使用了锁来访问共享内存
    也是足够用的。

    注：cpu memory barrier中某些类型的memory barrier需要成对使用，否则会出错。如一个写操作barrier
    需要和读操作(或者读数据依赖)barrier一起使用(当然，通用barrier也是可以的)，通常期望写barrier之前
    的Stores与读barrier或数据依赖屏障之后的Load相匹配，反之亦然：

    Intel上的wmb()宏实际上更简单，因为它展开为barrier(). 这是因为Intel处理器从不对写内存访问重新
    排序，因此，没有必要在代码中插入一条串行话汇编指令。不过，这个宏禁止编译期重新组合指令。

    最后，正确使用内存和优化屏障需要高度的技巧。因此应该注意到，一些内核维护人员不怎么喜欢内存屏障，
    使得该特性的代码很难进入到内核的主流版本中。因此，首先试着看一下是否能够在没有屏障的情况下完成
    工作，永远是值得的。这是可能的，因为在很多体系结构中上锁指令也相当于内存屏障。

    我的看法：
    内存屏障因为属于较为底层的同步原语，非常依赖CPU的支持程序。在linux内核代码中，实现的过程也是
    针对不同的CPU架构做了适配。所以对应用层来说，如果使用该同步原语，需要对代码运行的物理环境非常
    了解，并做适配，代码可迁移性很差。相比之下，c++11提供的原子变量的内存模型版本的fence封装性更好，
    建议使用。

参考：1. http://lxr.linux.no/linux+v2.6.24/Documentation/memory-barriers.txt
     2. http://www.rdrop.com/users/paulmck/scalability/paper/whymb.2010.06.07c.pdf
     3. https://www.cnblogs.com/my_life/articles/5220172.html
     4. https://www.cnblogs.com/straybirds/p/8856726.html
     5. 《深入Linux内核架构》
     6. 《深入理解Linux内核》

     -- yankefei
```

```c++
c++11原子操作的一些函数介绍：

part1:
c++11的原子"比较/交换"操作, 伪代码如下：
    bool compare_and_swap(int* a_obj, int* expected, int val)
    {
        if (*a_obj == expected)
        {
            *a_obj = val;
            return true;
        }
        else
        {
            *expected = *a_obj;
            return false;
        }
    }

如果预期值和obj一致的话，返回true, 并将obj的内容修改为val. 如果不一致，返回false， 将预期值修改
为obj的内容。
后缀是*_strong的版本执行的结果一定如此，但后缀是*_weak的版本与后缀是*_strong 不同，即使 *expected
确实比较等于 a_obj 中包含的值，这个弱版本也可以通过返回 false 来虚假失败。 对于某些循环算法，这可能
是可接受的行为，并且可能会在某些平台上显着提高性能。 在这些虚假失败中，该函数返回 false 而不会修改
预期（如下面的例子）。对于非循环算法，通常首选 *_strong。

    {
        extern atomic<bool> b; // 设置些什么
        bool expected = b.load();
        while(!b.compare_exchange_weak(expected,true)); // 最终结果一定会将b设置为true
    }


part2:
c++11的原子操作函数也提供了对应版本的内存屏障函数，
“如果原子操作库缺少了栅栏，那么这个库就是不完整的。” --《C++ Concurrency in Action》

void atomic_thread_fence (memory_order sync);


参考：
    1. http://www.cplusplus.com/reference/atomic
    2. cppreference-zh-20210212.chm
    3. 《C++ Concurrency in Action》Anthony Williams

    -- yankefei
```

```c++
c++11的原子操作内存模型介绍:

简单说，原子操作的内存模型将规定这样的一些准则，即所有原子类型的变量在多线程并发过程中的执行顺序。
这些内存模型是c++11在语言层面提供的一层封装，使用者将不需要额外关注如机器的硬件环境等额外因素，仅
按照自身需要选择这些从严格保证执行顺序：memory_order_seq_cst (默认)，到完全松散的执行顺序：
memory_order_relaxed 中的某一个即可。

1. memory_order_relaxed
    不对执行顺序做任何保证。在同一线程中对于同一变量的操作还是服从先发执行的关系，但是对不同线程几乎
    没有相对的顺序关系。唯一的要求是，在访问同一线程中的单个原子变量不能重排序；当一个给定线程已经看
    到一个原子变量的特定值，线程随后的读操作就不会去检索变量较早的那个值。

2. memory_order_acquire
    本线程中，所有后续的读操作必须在本条原子操作完成后执行。（读置后）

3. memory_order_release
    本线程中，所有之前的写操作完成后才能执行本条原子操作。（写置前）

4. memory_order_acq_rel
    同时包含memory_order_acquire和memory_order_release。

5. memory_order_consume
    本线程中，所有后续的有关**本原子类型**的操作，必须在本条原子操作完成之后执行。(本类型操作置后)
    最常见的使用方式就是一个指针使用该内存模型load一个原子类型的指针变量，那么后面对该指针的解引用
    操作必然会发生在该load操作之后。如果确认需要，std::kill_dependency()可以打断这种依赖关系。

6. memory_order_seq_cst
    全部存取都按顺序执行. 在一个多核的机器上，它会加强对性能的惩罚，因为整个序列中的操作都必须在
    多个处理器上保持一致，可能需要对处理器间的同步操作进行扩展（代价很昂贵）

有几点说明：
a: memory_order_release 和 memory_order_acquire常常结合使用，保证执行有序，内存顺序称为：
   release-acquire内存顺序。

b: memory_order_consume因为仅仅作用于本原子类型，对其他的原子类型和非原子类型没有实际影响，实际
   相比 memory_order_acquire 来说，"先于发生"的关系被弱化了。使用memory_order_release和
   memory_order_consume的配合会建立关于原子类型的“生产消费”型同步顺序，我们称为release-consume
   内存顺序。

c: memory_order_acq_rel, 则是常用于实现一种叫做CAS（compare and swap）的基本操作同步原语，对应
   到atomic的原子操作compare_exchange_strong成员函数上。我们也称为acquire-release内存顺序。

d: 原子存储操作（store）可以使用：memory_order_relaxed, memory_order_release, memory_order_seq_cst。
   
e: 原子读取操作（load） 可以使用：memory_order_relaxed，memory_order_acquire，memory_order_consume，
   memory_order_seq_cst。
   
f: RMW操作（read-modify-write），即一些需要同时读写的操作，比如atomic_flag的test_and_set操作，
   以及atomic类模板的atomic_compare_exchange()操作等都是需要同时读写的，可以使用所有6中内存模型。


综上， 顺序一致，松散，release-acquire，release-consume，acquire-release是五种常用的内存顺序。


参考：1. 《深入理解c++11, c++11新特性解析与应用》
     2. 《C++ Concurrency in Action》Anthony Williams
     (阅读建议：Anthony Williams的著作[参考2]内容比较晦涩难懂，因为他假设你对并发编程已经有了一些了解，
     建议先阅读国内作者的著作[参考1]，对内容有了一定了解后，再去阅读[参考2]的第5. 7章, 会容易很多)

     -- yankefei
```