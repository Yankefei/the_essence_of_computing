#include "stream.h"

#include "../lock_free_container/kfifo.h"

#include <stdio.h>

using namespace tools;

int main()
{
	{
		// 定义
		DECLARE_KFIFO(my_fifo, char, 1024);

		INIT_KFIFO(my_fifo);
	}
	{
		DEFINE_KFIFO(my_fifo2, char, 1024);
	}

	{
		struct _kfifo  my_fifo3;
		if (ESUCCESS != kfifo_alloc(&my_fifo3, 1024))
			stream << "alloc error" << std::endl;
		kfifo_free(&my_fifo3);

		stream << "struct _kfifo : " << sizeof(my_fifo3) << std::endl;
		stream << "struct __kfifo : " << sizeof(struct __kfifo) << std::endl;

		stream << "struct kfifo_rec_ptr_1 : " << sizeof(struct kfifo_rec_ptr_1) << std::endl;
		stream << "struct kfifo_rec_ptr_2 : " << sizeof(struct kfifo_rec_ptr_2) << std::endl;

		// 一个小技巧：在指针内部保存了长度信息
		char (*ptr1)[0];
		char (*ptr2)[1];
		stream << "sizeof *ptr1 " << sizeof(*ptr1) << std::endl;  // 0
		stream << "sizeof *ptr2 " << sizeof(*ptr2) << std::endl;  // 1
	}

	{
		struct _kfifo  my_fifo4[4];
		if (ESUCCESS != kfifo_alloc(my_fifo4 + 1, 1024))
			stream << "alloc error" << std::endl;
		kfifo_free(my_fifo4 + 1);
	}


    return 0;
}
