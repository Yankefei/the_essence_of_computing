#include <stdio.h>

struct __kfifo {
	unsigned int	in;
	unsigned int	out;
	unsigned int	mask;
	unsigned int	esize;
	void		*data;
};

struct kfifo
{
	union { \
		struct __kfifo	kfifo; \
		unsigned char	*type; \
		const unsigned char	*const_type; \
		char		(*rectype)[0]; \
		void		*ptr; \
		void const	*ptr_const; \
	};
    char buf[0];
};

int main()
{
    struct kfifo  my_test;

    return 0;
}
