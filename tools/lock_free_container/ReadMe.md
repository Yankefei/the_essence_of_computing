```c++
memory barriers, 内存屏障简介：

由于现代大多数CPU的指令执行序列可能乱序，原因一部分来自多核CPU执行，一部分来自编译期的优化，为了
保证指令顺序执行，内存屏障发挥了很重大的作用。

memory barriers 包含两类
1. 编译器 barrier.  (又称优化barrier)，用于避免编译期优化带来的内存乱序访问的问题
    linux内核实现的barrier() 如下 (x86-64架构)
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

    所有的cpu memory barrier都隐含了编译期barrier.
    linux内核的代码会根据配置在单cpu使用编译器barrier, 而在SMP上才使用memory barrier. 另外在SMP
    上如果使用共享内存，也必须使用memory barrier而不是编译期barrier. 当然，使用了锁来访问共享内存
    也是足够用的。

    注：cpu memory barrier中某些类型的memory barrier需要成对使用，否则会出错。如一个写操作barrier
    需要和读操作(或者读数据依赖)barrier一起使用(当然，通用barrier也是可以的)，反之亦然。


    最后，正确使用内存和优化屏障需要高度的技巧。因此应该注意到，一些内核维护人员不怎么喜欢内存屏障，
    使得该特性的代码很难进入到内核的主流版本中。因此，首先试着看一下是否能够在没有屏障的情况下完成
    工作，永远是值得的。这是可能的，因为在很多体系结构中上锁指令也相当于内存屏障。

参考：1. http://lxr.linux.no/linux+v2.6.24/Documentation/memory-barriers.txt
     2. https://www.cnblogs.com/my_life/articles/5220172.html
     3. https://www.cnblogs.com/straybirds/p/8856726.html
     4. 《深入Linux内核架构》
```

```c++

```