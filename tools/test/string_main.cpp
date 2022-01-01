#include <iostream>
#include <string>

#include "stream.h"
#include "string.hpp"

using namespace tools;

/*
https://blog.csdn.net/magiclyj/article/details/72922150

其实字节对齐的细节和具体编译器实现相关，但一般而言，满足三个准则：
1) 结构体变量的首地址能够被其最宽基本类型成员的大小所整除；
2) 结构体每个成员相对于结构体首地址的偏移量都是成员大小的整数倍，如有需要编译器会在成员之间加上填充字节；
   例如上面第二个结构体变量的地址空间。
3) 结构体的总大小为结构体最宽基本类型成员大小的整数倍，如有需要编译器会在最末一个成员之后加上填充字节。

在C++类里在没有任何数据类型变量的时候，会有一个字节的空间占用，如果有数据类型变量就会有字节对齐，
*/

struct EmptyClass {};

struct TestClass
{
    //EmptyClass  e_;
    //int t = 0;

    // 分析 std::string 源码的过程中发现了一个问题：
    // union 如果匿名，则相当于定义在结构体中
    // struct的表现同union
    struct
    {
        char array[8];
        int  a;
    };
};

// 带调试输出的最小 C++11 分配器
template <class Tp>
struct NAlloc {
    typedef Tp value_type;
    NAlloc() = default;
    template <class T> NAlloc(const NAlloc<T>&) {}
    Tp* allocate(std::size_t n)
    {
        n *= sizeof(Tp);
        std::cout << "allocating " << n << " bytes\n";
        return static_cast<Tp*>(::operator new(n));
    }
    void deallocate(Tp* p, std::size_t n)
    {
        std::cout << "deallocating " << n*sizeof(Tp) << " bytes\n";
        ::operator delete(p);
    }
};


template <class T, class U>
bool operator==(const NAlloc<T>&, const NAlloc<U>&)
{ return true; }
template <class T, class U>
bool operator!=(const NAlloc<T>&, const NAlloc<U>&)
{ return false; }



int main()
{
    {
        stream << "sizeof std::string   : "<< sizeof(std::string) << std::endl;

        tools::String a;
        stream << "sizeof tools::String : "<< sizeof(tools::String) << std::endl;
    }

    {
        tools::String  test("123");
        test.print();

        test += "456";
        test += '7';
        test.print();

        tools::String test1(test);
        //test1.print();

        stream <<"1 size: "<< test.size() <<", cap: "<< test.capacity() <<" "<< test.c_str() << std::endl;
        
        test = test1;
        test1 += "abc";

        //test.print();
        stream <<"size: "<< test.size() <<", cap: "<< test.capacity() <<" "<< test.c_str() << std::endl;
        test += '0';
        stream <<"size: "<< test.size() <<", cap: "<< test.capacity() <<" "<< test.c_str() << std::endl;

        test.pop_back();
        stream <<"size: "<< test.size() <<", cap: "<< test.capacity() <<" "<< test.c_str() << std::endl;

        test.swap(test1);
        stream <<"after swap size: "<< test.size() <<", cap: "<< test.capacity() <<" "<< test.c_str() << std::endl;

    }

    {
        tools::String test1("1");
        test1.push_back('2');
        test1 += "345";

        stream <<"test1: size: "<< test1.size() <<", cap: "<< test1.capacity() <<" "<< test1.c_str() << std::endl;

        test1.insert(3, 'z');
        stream <<"test1: size: "<< test1.size() <<", cap: "<< test1.capacity() <<" "<< test1.c_str() << std::endl;

        test1.insert(1, "abc");
        stream <<"test1: size: "<< test1.size() <<", cap: "<< test1.capacity() <<" "<< test1.c_str() << std::endl;

        stream << "after erase" << std::endl;
        test1.erase(test1.begin());
        test1.erase(test1.begin() + 1);
        stream <<"test1: size: "<< test1.size() <<", cap: "<< test1.capacity() <<" "<< test1.c_str() << std::endl;

        test1.erase(test1.begin() + 2, test1.begin() + 5);
        stream <<"test1: size: "<< test1.size() <<", cap: "<< test1.capacity() <<" "<< test1.c_str() << std::endl;


        test1.append("mnbvc");
        stream <<"test1: size: "<< test1.size() <<", cap: "<< test1.capacity() <<" "<< test1.c_str() << std::endl;

        auto index = test1.find('m');
        stream << "index of m : "<< index << std::endl;

        index = test1.find("mnb");
        stream << "index of mnb: "<< index << std::endl;

        index = test1.find("mnbvc");
        if (index == tools::String::npos)
            stream << "index of mnbvc: failed" << std::endl;
        else
            stream << "index of mnbvc: "<< index << std::endl;

        tools::String test2 = test1;
        test2[5] = 'p';
        stream <<"test2: size: "<< test2.size() <<", cap: "<< test2.capacity() <<" "<< test2.c_str() << std::endl;

        stream <<"is > :"<< ( test2 > test1 ? "true": "false") << std::endl;

        tools::String test3 = test1;

        stream << "is same: "<< (test3 == test1 ? "true": "false") << std::endl;
   
        stream << test3 << std::endl;
    }

    {
        tools::BString<char, NAlloc<char>> a_other("123");

        a_other += "456";
        stream << "a_other: " << a_other << std::endl;

        //Print(a_other);
        // stream << "a_other, size: "<< a_other.size() << ": "
        //        << a_other << std::endl;
    }

    return 0;
}