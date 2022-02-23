#include "stream.h"
#include <cstring>

using namespace tools;


struct MyStruct
{
	int a;
	std::string b;
	std::vector<int32_t> c;
};

void func1(const std::string& name,  std::string& str)
{
	stream << str << "\n";
}

//void func2(const std::string& name, const void* ptr)
//{
//}

void func3(const std::string& name,  uint64_t& a)
{
	stream << a << "\n";
}

void func4(const std::string& name,  std::vector<std::string>& a)
{
	stream << a.size() << "\n";
	for (auto e : a)
	{
		stream << " e: " << e << "\n";
	}
}

void func5(const std::string& name,  MyStruct & a)
{
	stream << a.a << "\n";
	stream << a.b << "\n";
	stream << a.c.size() << "\n";
}


/**
 *  这是一个很有趣的编程技法，如果现在有多个函数指针，参数的个数相同，但是最后一个参数的类型不一致
 *  有一种方式可以用一个函数指针的定义式调用其他所有的回调函数：
 *  如下所示，将最后一个参数定义为 const void* str, 调用的时候，将其他函数指针拷贝过来，然后传入
 *  实际调用类型参数的地址即可。可以做到函数调用的形式一致性。目前测试，可以支持任意类型的const
 *  修饰的参数.
 *  ps. 经过测试发现，不加const修饰可完全可以
*/
typedef void(*func2_type)(const std::string& name,  void* str);

typedef void(*func1_type)(const std::string& name,  std::string& str);
typedef void(*func3_type)(const std::string& name,  uint64_t& a);
typedef void(*func4_type)(const std::string& name,  std::vector<std::string>& a);
typedef void(*func5_type)(const std::string& name,  MyStruct& a);


void test()
{
	func1_type func1_local = func1;

	func2_type  func2_local;

	memcpy(reinterpret_cast<void*>(&func2_local), reinterpret_cast<void*>(&func1_local), sizeof(func2_type));

	std::string str = "value";

	func2_local("123", &str);
}

void test2()
{
	func3_type func3_local = func3;

	func2_type  func2_local;

	memcpy(reinterpret_cast<void*>(&func2_local), reinterpret_cast<void*>(&func3_local), sizeof(func2_type));

	uint64_t test_a = 123456;

	func2_local("123", &test_a);
}

void test3()
{
	func4_type func4_local = func4;

	func2_type  func2_local;

	memcpy(reinterpret_cast<void*>(&func2_local), reinterpret_cast<void*>(&func4_local), sizeof(func2_type));

	std::vector<std::string> v{ "1","12", "123", "1234" };

	func2_local("123", &v);
}

void test4()
{
	func5_type func5_local = func5;

	func2_type  func2_local;

	memcpy(reinterpret_cast<void*>(&func2_local), reinterpret_cast<void*>(&func5_local), sizeof(func2_type));

	MyStruct v;
	v.a = 100;
	v.b = "yankefei";
	v.c = std::vector<int32_t>({ 123, 123, 123 });

	func2_local("123", &v);
}

int main()
{
	test();
	test2();
	test3();
	test4();

	return 0;
}


#if 0

struct MyStruct
{
	int a;
	std::string b;
	std::vector<int32_t> c;
};

void func1(const std::string& name, const std::string& str)
{
	stream << str << "\n";
}

//void func2(const std::string& name, const void* ptr)
//{
//}

void func3(const std::string& name, const uint64_t& a)
{
	stream << a << "\n";
}

void func4(const std::string& name, const std::vector<std::string>& a)
{
	stream << a.size() << "\n";
	for (auto e : a)
	{
		stream << " e: " << e << "\n";
	}
}

void func5(const std::string& name, const MyStruct & a)
{
	stream << a.a << "\n";
	stream << a.b << "\n";
	stream << a.c.size() << "\n";
}


/**
 *  这是一个很有趣的编程技法，如果现在有多个函数指针，参数的个数相同，但是最后一个参数的类型不一致
 *  有一种方式可以用一个函数指针的定义式调用其他所有的回调函数：
 *  如下所示，将最后一个参数定义为 const void* str, 调用的时候，将其他函数指针拷贝过来，然后传入
 *  实际调用类型参数的地址即可。可以做到函数调用的形式一致性。目前测试，可以支持任意类型的const
 *  修饰的参数
*/
typedef void(*func2_type)(const std::string& name, const void* str);

typedef void(*func1_type)(const std::string& name, const std::string& str);
typedef void(*func3_type)(const std::string& name, const uint64_t& a);
typedef void(*func4_type)(const std::string& name, const std::vector<std::string>& a);
typedef void(*func5_type)(const std::string& name, const MyStruct& a);


void test()
{
	func1_type func1_local = func1;

	func2_type  func2_local;

	memcpy(reinterpret_cast<void*>(&func2_local), reinterpret_cast<void*>(&func1_local), sizeof(func2_type));

	std::string str = "value";

	func2_local("123", &str);
}

void test2()
{
	func3_type func3_local = func3;

	func2_type  func2_local;

	memcpy(reinterpret_cast<void*>(&func2_local), reinterpret_cast<void*>(&func3_local), sizeof(func2_type));

	uint64_t test_a = 123456;

	func2_local("123", &test_a);
}

void test3()
{
	func4_type func4_local = func4;

	func2_type  func2_local;

	memcpy(reinterpret_cast<void*>(&func2_local), reinterpret_cast<void*>(&func4_local), sizeof(func2_type));

	std::vector<std::string> v{ "1","12", "123", "1234" };

	func2_local("123", &v);
}

void test4()
{
	func5_type func5_local = func5;

	func2_type  func2_local;

	memcpy(reinterpret_cast<void*>(&func2_local), reinterpret_cast<void*>(&func5_local), sizeof(func2_type));

	MyStruct v;
	v.a = 100;
	v.b = "yankefei";
	v.c = std::vector<int32_t>({ 123, 123, 123 });

	func2_local("123", &v);
}

int main()
{
	test();
	test2();
	test3();
	test4();

	return 0;
}

#endif


#if 0

class Message
{
public:
	Message() {}
	virtual ~Message() {}


	virtual void* data() 
	{
		stream << "Message data" << std::endl;
		return 0;
	}

	virtual std::size_t get_size()
	{
		stream << "Message get_size" << std::endl;
		return 0;
	}
};

class AmiMessage : public Message
{
public:
	AmiMessage() {}
	virtual ~AmiMessage() {}

	virtual void* data()
	{
		stream << "AmiMessage data" << std::endl;
		return 0;
	}

	virtual std::size_t get_size()
	{
		stream << "AmiMessage get_size" << std::endl;
		return 0;
	}
};

class AmiMessage2 : public AmiMessage
{
public:
	AmiMessage2() {}
	virtual ~AmiMessage2() {}

	virtual void* data()
	{
		stream << "AmiMessage2 data" << std::endl;
		return 0;
	}

	virtual std::size_t get_size()
	{
		stream << "AmiMessage2 get_size" << std::endl;
		return 0;
	}
};


void func(Message* ptr)
{
	ptr->data();
	ptr->get_size();
}

int main()
{
	AmiMessage* msg = new AmiMessage();
	func(msg);
	delete msg;

	AmiMessage  msg2;
	func(&msg2);

	AmiMessage2 msg3;
	func(&msg3);

	return 0;
}


#endif