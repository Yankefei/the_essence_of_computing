#include <iostream>

#include <cstring>
#include <memory>
#include "stream.h"
#include "vector.hpp"

using namespace tools;
using namespace std;



int main()
{
	std::shared_ptr<int> a(new int(4));
	std::shared_ptr<int> b = a;
	a.reset();

	printf("%d\n", a.use_count());
	printf("%d\n", b.use_count());

	printf("%d\n", a);
	printf("%d\n", b);


	char array[] = "hello";
	char array2[] = {"hello"};

	assert(strcmp(array, array2) == 0);

	stream << sizeof(array) <<  ", "<<strlen(array) << std::endl;
	stream << sizeof(array2) <<  ", "<<strlen(array2) << std::endl;

	/*
		free delete 空指针竟然没事
	*/
	int *p1 = nullptr;
	delete p1;

	int *p2 = nullptr;
	free(p2);

	// int *pp = new int(10);
	// delete pp;
	// delete pp;


	// int* p = (int*)malloc(sizeof(int));
	// free(p);
	// free(p);



	return 0;
}

#if 0

class Test
{
private:
	int value;
public:
	Test()
	{
		memset(this,0,sizeof(Test));
	}
	~Test() {}
	virtual void fun() { cout<<"Test::fun"<<endl;}
	void print() { cout<<"Test::print: "<<value<<endl;}
};
int main()
{
	Test t1;
	t1.fun();
	t1.print();
	Test *p = &t1;
	p->print();
	p->fun();   // core
}



class Object
{
	int value;
public:
	Object(int x=0):value(x) 
	{

	}
	Object()
	{
	}
	virtual void add() { cout<<"Object::add"<<endl;}
	virtual void fun() const { cout<<"Object::fun"<<endl;}
	virtual void print() { cout<<"Object::print"<<endl;}
};
class Base : public Object
{
	int max;
public:
	Base(int x=0):max(x+10),Object(x)
	{
		
	}
	~Base()
	{
	}

	void lfq() // this
	{

	}
	virtual void add() { cout<<"Base::add"<<endl;}
	virtual void fun() const { cout<<"Base::fun"<<endl;}
	virtual void yhp() { cout<<"Base::print"<<endl;}
};

class Test : public Base
{
	int num;
public:
	Test(int x=0):Base(x),num(x+10)
	{
	}
	~Test()
	{
	}
	virtual void add() { cout<<"Test::add"<<endl;}
	virtual void print() { cout<<"Test::print"<<endl;}
	virtual void yhp() { cout<<"Test::yhp"<<endl;}
};

int main()
{
	Test t1(10);

	Object obj1(10);

	Object *op1 = &t1;   // yhp
	Object *op2 = &obj1; 
	((Test*)op1)->yhp();  // yhp
	((Test*)op2)->yhp();  // core   无效的向下转换类型
}

class Test
{
private:
	int value;
public:
	Test(int x=0):value(x)
	{
	}
	void fun() { cout<<"Test::fun"<<endl;}
	void print() const 
	{
		cout<<"Test::parint value: "<<value<<endl;
	}
};

int main()
{
	Test t1;
	Test *p = (Test*)malloc(sizeof(Test));
	p->fun();
	//p->print();

	t1.print();
	// *p = t1;         // 没有依存一个合格的对象，但是没有使用到虚函数表，只要开辟了内存，就安全了
	p->fun(); // fun(p);
	p->print(); // print(p);
}

#endif
#if 0

class Test
{
private:
	int value;
public:
	Test(int x=0):value(x)
	{
	}
	void fun() { cout<<"Test::fun"<<endl;}
	virtual void print() const 
	{
		cout<<"Test::parint value: "<<value<<endl;
	}
};

int main()
{
	Test t1;
	Test *p = nullptr;
	//Test *p = (Test*)malloc(sizeof(Test));
	
	p->fun();
	//p->print();

	t1.print();
	// *p = t1;         // 没有依存一个合格的对象， 即使malloc了空间，还是会core, 虚函数表为空
	//new(p) Test(t1);  // 使用定位new就没事
	p = &t1;           // 即使是是一个空指针，指向了一个完整对象就可以
	p->fun(); // fun(p);
	p->print(); // print(p);
}


#endif
#if 0

class Test
{
private:
	int value;
public:
	Test(int x=0):value(x)
	{
		//memset(this,0,sizeof(Test));
	}
    //void fun(Test * const this)
	void fun() { cout<<"Test::fun"<<endl;}
	//void print(const Test * const this)
	virtual void print() const 
	{
		cout<<"Test::parint value: "<<value<<endl;
	}
};

// int main()
// {
// 	Test t1;
// 	Test *p = &t1;//(Test*)malloc(sizeof(Test));
// 	t1.print();
// //	p->fun(); // fun(p);
// 	p->print(); // print(p);
// }

// int  main()
// {
// 	Test *p = NULL;
// 	p->fun(); // fun(p);
// 	p->print(); // print(p);
// }


class Test
{
	int value;
	int num;
public:
	Test(int x = 0):num(x),value(num) {} // 类成员变量按照定义的顺序构造，而不是初始化列表定义的顺序构造
	void print() const
	{
		cout<<"num: "<<num<<endl;      // 10
		cout<<"value: "<<value<<endl;  // value: 32767  无效值
	}
};

class Object
{
};
class Base 
{
};
class Test2
{
public:
	Base b1;
	Object obj1;
};

int main()
{
	stream << sizeof(Test2) << std::endl;

	Test t1(10);
	t1.print();

	return 0;
}


// 位段的类型只能是int，unsigned int，signed int三种类型，不能是char型或者浮点型；
// 无名位段不能被访问，但是会占据空间

union U
{
	unsigned  int aa;
	struct 
	{
		unsigned int bb1 : 7;
		unsigned int bb2 : 7;
		unsigned int bb3 : 1;
		unsigned char bb4 : 1;
	};
};


typedef struct{
    unsigned int a:1; //存在一个非0位的位段，则至少占4Byte,注意是至少
}s;
typedef struct {
    uint64_t a:33;  //这个占8字节
}ss;
typedef struct {
    unsigned int :0;  //存在一个0位的位段，C编译器占0字节，C++编译器占1字节
}s1;
typedef struct {
    unsigned int a:1;
    unsigned :0;          //下一个位段放在一个新的位段存储单元 ，所以占4+4=8Byte
    unsigned int b:2;
}s2;
typedef struct {
    unsigned int a:4;
    unsigned int b:32;  //由于4+32》32，所以b放在一个新的位段中 4+4=8字节
}s3;
typedef struct {
    unsigned int a:1;
    char b;  //这个加起来总共不超过4字节，占一个位段
    unsigned int : 1;
    unsigned int d:2;
    unsigned int e:2;
}S4;
extern int testBit()
{
    S4 s4;
    //s4.a=1;
    s4.b='a';
    s4.d=2;
    s4.e=2;
    cout<<s4.a<<"\t"<<s4.b<<"\t"<<s4.d<<"\t"<<s4.e<<endl;
    printf("sizeof(s)=%d\nsizeof(s1)=%d\nsizeof(s2)=%d\nsizeof(s3)=%d\nsizeof(s4)=%d\n",
        sizeof(s),sizeof(s1),sizeof(s2),sizeof(s3),sizeof(s4));
    cout<<sizeof(ss)<<endl;
    return 0;
}

int main() 
{ 
	stream << "sizeof(U): "<< sizeof(U) << std::endl;
	U a;
	a.bb1 = 1;
	a.bb2 = 1;
	a.bb3 = 1;

	stream << "sizeof(U): "<< sizeof(U) << std::endl;
	return 0;
}





struct Node
{
	char ch;
	typedef union
	{
		int i;
		float f;
	}IF;
    IF a;
};

int main()
{
	Node x;
    x.ch = 1;

	cout<<sizeof(x)<<endl;  // 8 字节对齐
	
}


class Test
{
private:
	int value;
public:
	Test(int x=0):value(x)
	{
	}
	void fun() { cout<<"Test::fun"<<endl;}
	virtual void print() const 
	{
		cout<<"Test::parint value: "<<value<<endl;
	}
};

int main()
{
	Test t1;
	Test *p  = (Test*)malloc(sizeof(Test));
	t1.print();
	
    //*p = t1;   //  还是会core, 就是这个指针没有依存与一个合格的对象
    p = &t1;    // 不会core, 依存于一个正常的对象
	//  new(p) Test(t1);  // 定位new, 调用了构造函数
	
    p->fun(); // fun(p);
	p->print(); // print(p);
}


class Test
{
private:
	int value;
public:
	Test(int x=0):value(x)
	{
        // 是否清空
		memset(this,0,sizeof(Test));
	}
    //void fun(Test * const this)
	void fun() { cout<<"Test::fun"<<endl;}
	//void print(const Test * const this)

    // 下面的函数通过增加了一个virtual， 让类的大小从4变到了16
	virtual void print()      
	{
		cout<<"Test::parint value: "<<value<<endl;
	}
};


int main()
{
    stream <<sizeof(Test) << std::endl;

	Test *p = new Test();
    Test t1;
	t1.print();   // 非指针的方式调用不会core，不会触发多态，也就是寻找虚函数表吧
	p->fun(); // fun(p);
	p->print(); // print(p);    只要不清空，就没事，清空会core
}



int main()
{
	Test t1;
	Test *p = (Test*)malloc(sizeof(Test));
	t1.print();
	p->fun(); // fun(p);
	p->print(); // print(p);  // 无论是否清空，都会core， 没有调用构造函数
}

int main()
{
	Test t1;
	Test *p = &t1;//(Test*)malloc(sizeof(Test));
	t1.print();               // 把类清空，这个t1.print是不会core的  非指针的方式调用，不会触发多态，也就是寻找虚函数表吧
	p->fun(); // fun(p);
	p->print(); // print(p);  // 只要把类内容清空，就会core
}

int main()
{
	Test *p = NULL;
	p->fun(); // fun(p);
	p->print(); // print(p);   // 携带了virtual, 运行就会core
}



typedef char ElemType;
#define END '#'

typedef struct BtNode
{
	BtNode *parent;
	BtNode *leftchild;
	BtNode *rightchild;
	ElemType data;
}BtNode , *BinaryTree;

BtNode * _Buynode()
{
	BtNode *s = (BtNode *)malloc(sizeof(BtNode));
	if(NULL == s) exit(1);
	memset(s,0,sizeof(BtNode));
	return s;
}
void _Freenode(BtNode *p)
{
	free(p);
}

void Print(BtNode* ptr)
{
    if (!ptr) return;

    stream << ptr->data << " ";
    Print(ptr->leftchild);
    Print(ptr->rightchild);
}


BtNode * Create(ElemType *str,int &i,int n,BtNode *pa)
{
	BtNode *s = NULL;
	if(i<n && str[i] != END)
	{
		s = _Buynode();
		s->data = str[i];
		s->parent = pa;
		s->leftchild = Create(str,++i,n,s);
		s->rightchild = Create(str,++i,n,s);
	}
	return s;
}
BtNode * CreateTree(ElemType *str,int n)
{
	if(str == NULL || n < 1) return NULL;
	else 
	{
		int i = 0;
		return Create(str,i,n,NULL);
	}
}

int  main()
{
	ElemType str[] = "ABC##DE##F##G#H##";
	int n = sizeof(str)/sizeof(str[0]);

	BinaryTree root = NULL;
	root = CreateTree(str,n);


    Print(root); stream << std::endl;
}


BtNode * Create(ElemType *&str,BtNode *pa)
{
	BtNode *s = NULL;
	if(str != NULL && *str != END)
	{
		s = _Buynode();
		s->data = *str;
		s->parent = pa;
		s->leftchild = Create(++str,s);
		s->rightchild = Create(++str,s);
	}
	return s;
}
BtNode * CreateTree(ElemType *str)
{
	return Create(str,NULL);
}

int main()
{
	ElemType str[] = "ABC##DE##F##G#H##";
	int n = sizeof(str)/sizeof(str[0]) -1;

	BinaryTree root = NULL;
	root = CreateTree(str);

    Print(root); stream << std::endl;

}



class Object
{
};
class Base 
{
};
class Test
{
public:
	Base b1;
	Object obj1;
};

class Test2
{
public:
	Base b1;
};
int main()
{
	stream <<sizeof(Test) << std::endl;    // 2
    stream <<sizeof(Test2) << std::endl;  // 1
}




void Print2Max(int ar[],int n)
{
	if(ar == NULL || n < 2) return ;//
	int max1 = ar[0]>ar[1]? ar[0]:ar[1];
	int max2 = ar[0]>ar[1]? ar[1]:ar[0];

	for(int i = 2; i<n;++i)
	{
		if(ar[i] > max1)
		{
			max2 = max1;
			max1 = ar[i];
		}else if(ar[i] > max2)
		{
			max2 = ar[i];
		}
	}
	stream <<"Max1: "<<max1<< std::endl;
	stream <<"Max2: "<<max2<< std::endl;
}

int main()
{
	int ar[]={12,45,67,90,78,67,56,34,23,100};
	int n = sizeof(ar)/sizeof(ar[0]);

	Print2Max(ar,n);
}


class Test
{
private:
	int value;
	static int num;
public:
	Test(int x=0):value(x)
	{
	}
	~Test(){}

	// void fun(Test * const this)
	void fun()
	{
		this->value = 10;
		num = 10;
	}
	// void func(const Test * const this)
	void func() const
	{
	    // this->fun();// fun(this)
		int x = this->value;
		num = 10; // 可以访问类内的静态成员变量
	}

    // 类的静态成员方法可以访问类内的私有成员变量
	static void print(Test &it)
	{
	    it.func();
		it.value =10;
		num = 10;
	}
    // 友元函数也可以访问类内的静态成员以及私有成员变量
	friend void show(Test &it);
};
int Test::num = 0;

void show(Test &it)
{
	it.num = 0;
	Test::num = 0;
	it.value = 0;
    it.func();
}


int main()
{
	Test t1(10),t2(20);
}


#endif