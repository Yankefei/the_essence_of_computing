#include <iostream>

#include <cstring>
#include "stream.h"
#include "vector.hpp"

using namespace tools;
using namespace std;

int main() { return 0; }
#if 0

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