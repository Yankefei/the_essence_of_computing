/*
 * A generic kernel FIFO implementation
 *
 * Copyright (C) 2013 Stefani Seibold <stefani@seibold.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef _TOOLS_LINUX_KFIFO_H
#define _TOOLS_LINUX_KFIFO_H

/*
 * How to porting drivers to the new generic FIFO API:
 *
 * - Modify the declaration of the "struct kfifo *" object into a
 *   in-place "struct kfifo" object
 * - Init the in-place object with kfifo_alloc() or kfifo_init()
 *   Note: The address of the in-place "struct kfifo" object must be
 *   passed as the first argument to this functions
 * - Replace the use of __kfifo_put into kfifo_in and __kfifo_get
 *   into kfifo_out
 * - Replace the use of kfifo_put into kfifo_in_spinlocked and kfifo_get
 *   into kfifo_out_spinlocked
 *   Note: the spinlock pointer formerly passed to kfifo_init/kfifo_alloc
 *   must be passed now to the kfifo_in_spinlocked and kfifo_out_spinlocked
 *   as the last parameter
 * - The formerly __kfifo_* functions are renamed into kfifo_*
 */

/* 无锁声明
 * Note about locking : There is no locking required until only * one reader
 * and one writer is using the fifo and no kfifo_reset() will be * called
 *  kfifo_reset_out() can be safely used, until it will be only called
 * in the reader thread.
 *  For multiple writer and one reader there is only a need to lock the writer.
 * And vice versa for only one writer and multiple reader there is only a need
 * to lock the reader.
 */


#include "general.h"
#include "spin_lock.h"
#include <stdint.h>

#define ESUCCESS   0

#define	EINVAL		22	/* Invalid argument */
#define	ENOMEM		12	/* Out of memory */


#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define __must_check            __attribute__((warn_unused_result))

// 参考：1. https://lwn.net/Articles/347619/
//       2. https://blog.csdn.net/Bruno_Mars/article/details/100061793

struct __kfifo {
	unsigned int	in;
	unsigned int	out;
	unsigned int	mask;   // 表示队列的长度 - 1, 不直接定义队列总长度很有趣
	unsigned int	esize;
	void		*data;
};

/**
 * 提前设置好一个ptrtype的类型，用于后面的typeof获取类型
 * rectype 用这样一个指向数组的指针就可以定义出块数据的长度信息，而不额外占用内存，厉害。内部
 * 除了kfifo之外的其他字段，均只涉及到类型信息，而没有使用具体的数据
*/
#define __STRUCT_KFIFO_COMMON(datatype, recsize, ptrtype) \
	union { \
		struct __kfifo	kfifo; \
		datatype	*type; \
		const datatype	*const_type; \
		char		(*rectype)[recsize]; \
		ptrtype		*ptr; \
		ptrtype const	*ptr_const; \
	}

#define __STRUCT_KFIFO(type, size, recsize, ptrtype) \
{ \
	__STRUCT_KFIFO_COMMON(type, recsize, ptrtype); \
	type		buf[((size < 2) || (size & (size - 1))) ? -1 : size]; \
}

#define STRUCT_KFIFO(type, size) \
	struct __STRUCT_KFIFO(type, size, 0, type)

#define __STRUCT_KFIFO_PTR(type, recsize, ptrtype) \
{ \
	__STRUCT_KFIFO_COMMON(type, recsize, ptrtype); \
	type		buf[0]; \
}

#define STRUCT_KFIFO_PTR(type) \
	struct __STRUCT_KFIFO_PTR(type, 0, type)

/*
 * define compatibility "struct kfifo" for dynamic allocated fifos
 */
// struct kfifo __STRUCT_KFIFO_PTR(unsigned char, 0, void);  g++ 无法编译， 修改结构体名称
// 默认队列内容为字符串
struct _kfifo __STRUCT_KFIFO_PTR(unsigned char, 0, void);


#define STRUCT_KFIFO_REC_1(size) \
	struct __STRUCT_KFIFO(unsigned char, size, 1, void)

#define STRUCT_KFIFO_REC_2(size) \
	struct __STRUCT_KFIFO(unsigned char, size, 2, void)

/*
 * define kfifo_rec types
 *  1 或者2字节表示固定长度记录，该参数指定与每个条目一起存储的长度字段的大小
 *  （即 1 或 2，尽管不支持 0 表示长度）
 */
struct kfifo_rec_ptr_1 __STRUCT_KFIFO_PTR(unsigned char, 1, void);
struct kfifo_rec_ptr_2 __STRUCT_KFIFO_PTR(unsigned char, 2, void);

/*
 * helper macro to distinguish between real in place fifo where the fifo
 * array is a part of the structure and the fifo type where the array is
 * outside of the fifo structure.
 */
#define	__is_kfifo_ptr(fifo)	(sizeof(*fifo) == sizeof(struct __kfifo))

/**
 * DECLARE_KFIFO_PTR - macro to declare a fifo pointer object
 * @fifo: name of the declared fifo
 * @type: type of the fifo elements
 */
#define DECLARE_KFIFO_PTR(fifo, type)	STRUCT_KFIFO_PTR(type) fifo

/**
 * DECLARE_KFIFO - macro to declare a fifo object
 * @fifo: name of the declared fifo
 * @type: type of the fifo elements
 * @size: the number of elements in the fifo, this must be a power of 2
 */
#define DECLARE_KFIFO(fifo, type, size)	STRUCT_KFIFO(type, size) fifo

/**
 * INIT_KFIFO - Initialize a fifo declared by DECLARE_KFIFO
 * @fifo: name of the declared fifo datatype
 * @note: 下面代码中所有的typeof都可被c++风格的decltype来替代
 */
#define INIT_KFIFO(fifo) \
(void)({ \
	typeof(&(fifo)) __tmp = &(fifo); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	__kfifo->in = 0; \
	__kfifo->out = 0; \
	__kfifo->mask = __is_kfifo_ptr(__tmp) ? 0 : ARRAY_SIZE(__tmp->buf) - 1; \
	__kfifo->esize = sizeof(*__tmp->buf); \
	__kfifo->data = __is_kfifo_ptr(__tmp) ?  NULL : __tmp->buf; \
})

/**
 * DEFINE_KFIFO - macro to define and initialize a fifo
 * @fifo: name of the declared fifo datatype
 * @type: type of the fifo elements
 * @size: the number of elements in the fifo, this must be a power of 2
 *
 * Note: the macro can be used for global and local fifo data type variables.
 */
#define DEFINE_KFIFO(fifo, type, size) \
	DECLARE_KFIFO(fifo, type, size) = \
	(typeof(fifo)) { \
		{ \
			{ \
			.in	= 0, \
			.out	= 0, \
			.mask	= __is_kfifo_ptr(&(fifo)) ? \
				  0 : \
				  ARRAY_SIZE((fifo).buf) - 1, \
			.esize	= sizeof(*(fifo).buf), \
			.data	= __is_kfifo_ptr(&(fifo)) ? \
				NULL : \
				(fifo).buf, \
			} \
		} \
	}


static inline unsigned int __must_check
__kfifo_uint_must_check_helper(unsigned int val)
{
	return val;
}

static inline int __must_check
__kfifo_int_must_check_helper(int val)
{
	return val;
}

/**
 * kfifo_initialized - Check if the fifo is initialized
 * @fifo: address of the fifo to check
 *
 * Return %true if fifo is initialized, otherwise %false.
 * Assumes the fifo was 0 before.
 */
#define kfifo_initialized(fifo) ((fifo)->kfifo.mask)

/**
 * kfifo_esize - returns the size of the element managed by the fifo
 * @fifo: address of the fifo to be used
 */
#define kfifo_esize(fifo)	((fifo)->kfifo.esize)

/**
 * kfifo_recsize - returns the size of the record length field
 * @fifo: address of the fifo to be used
 */
#define kfifo_recsize(fifo)	(sizeof(*(fifo)->rectype))

/**
 * kfifo_size - returns the size of the fifo in elements
 * @fifo: address of the fifo to be used
 */
#define kfifo_size(fifo)	((fifo)->kfifo.mask + 1)

/**
 * kfifo_reset - removes the entire fifo content
 * @fifo: address of the fifo to be used
 *
 * Note: usage of kfifo_reset() is dangerous. It should be only called when the
 * fifo is exclusived(独占的) locked or when it is secured（安全的） that no other thread is
 * accessing the fifo.
 * // 下面的宏定义，typeof((fifo) + 1) __tmp = (fifo); 之所以要 + 1, 是做了一个参数类型
 * // 检查，确保是指针，如果传递的结构体会产生编译错误，如果是数组名，而且兼容数组名，会退化
 * // 为指针类型
 */
#define kfifo_reset(fifo) \
(void)({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	__tmp->kfifo.in = __tmp->kfifo.out = 0; \
})

/**
 * kfifo_reset_out - skip fifo content
 * @fifo: address of the fifo to be used
 *
 * Note: The usage of kfifo_reset_out() is safe until it will be only called
 * from the reader thread and there is only one concurrent reader. Otherwise
 * it is dangerous and must be handled in the same way as kfifo_reset().
 */
#define kfifo_reset_out(fifo)	\
(void)({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	__tmp->kfifo.out = __tmp->kfifo.in; \
})

/**
 * kfifo_len - returns the number of used elements in the fifo
 * @fifo: address of the fifo to be used
 */
#define kfifo_len(fifo) \
({ \
	typeof((fifo) + 1) __tmpl = (fifo); \
	__tmpl->kfifo.in - __tmpl->kfifo.out; \
})

/**
 * kfifo_is_empty - returns true if the fifo is empty
 * @fifo: address of the fifo to be used
 */
#define	kfifo_is_empty(fifo) \
({ \
	typeof((fifo) + 1) __tmpq = (fifo); \
	__tmpq->kfifo.in == __tmpq->kfifo.out; \
})

/**
 * kfifo_is_full - returns true if the fifo is full
 * @fifo: address of the fifo to be used
 */
#define	kfifo_is_full(fifo) \
({ \
	typeof((fifo) + 1) __tmpq = (fifo); \
	kfifo_len(__tmpq) > __tmpq->kfifo.mask; \
})

/**
 * kfifo_avail - returns the number of unused elements in the fifo
 * @fifo: address of the fifo to be used
 */
#define	kfifo_avail(fifo) \
__kfifo_uint_must_check_helper( \
({ \
	typeof((fifo) + 1) __tmpq = (fifo); \
	const size_t __recsize = sizeof(*__tmpq->rectype); \
	unsigned int __avail = kfifo_size(__tmpq) - kfifo_len(__tmpq); \
	(__recsize) ? ((__avail <= __recsize) ? 0 : \
					__kfifo_max_r(__avail - __recsize, __recsize)) : \
				__avail; \
}) \
)

/** 单纯消耗一个长度，但是不返回
 * kfifo_skip - skip output data
 * @fifo: address of the fifo to be used
 */
#define	kfifo_skip(fifo) \
(void)({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	const size_t __recsize = sizeof(*__tmp->rectype); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	if (__recsize) \
		__kfifo_skip_r(__kfifo, __recsize); \
	else \
		__kfifo->out++; \
})

/**
 * kfifo_peek_len - gets the size of the next fifo record
 * @fifo: address of the fifo to be used
 *
 * This function returns the size of the next fifo record in number of bytes.
 * 如果没有提前设置record的长度，则返回剩余buffer的整个长度
 */
#define kfifo_peek_len(fifo) \
__kfifo_uint_must_check_helper( \
({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	const size_t __recsize = sizeof(*__tmp->rectype); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	(!__recsize) ? kfifo_len(__tmp) * sizeof(*__tmp->type) : \
	__kfifo_len_r(__kfifo, __recsize); \
}) \
)

/**
 * kfifo_alloc - dynamically allocates a new fifo buffer
 * @fifo: pointer to the fifo
 * @size: the number of elements in the fifo, this must be a power of 2
 * @gfp_mask: get_free_pages mask, passed to kmalloc()
 *
 * This macro dynamically allocates a new fifo buffer.
 *
 * The numer of elements will be rounded-up to a power of 2.
 * The fifo will be release with kfifo_free().
 * Return 0 if no error, otherwise an error code.
 */
#define kfifo_alloc(fifo, size) \
__kfifo_int_must_check_helper( \
({ \
	decltype((fifo) + 1) __tmp = (fifo); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	__is_kfifo_ptr(__tmp) ? \
	__kfifo_alloc(__kfifo, size, sizeof(*__tmp->type)) : \
	-EINVAL; \
}) \
)

/**
 * kfifo_free - frees the fifo
 * @fifo: the fifo to be freed
 */
#define kfifo_free(fifo) \
({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	if (__is_kfifo_ptr(__tmp)) \
		__kfifo_free(__kfifo); \
})

/**
 *  使用一个已经分配好的buffer来填充
 * kfifo_init - initialize a fifo using a preallocated buffer
 * @fifo: the fifo to assign the buffer
 * @buffer: the preallocated buffer to be used
 * @size: the size of the internal buffer, this have to be a power of 2
 *
 * This macro initialize a fifo using a preallocated buffer.
 *
 * The numer of elements will be rounded-up to a power of 2.
 * Return 0 if no error, otherwise an error code.
 */
#define kfifo_init(fifo, buffer, size) \
({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	__is_kfifo_ptr(__tmp) ? \
	__kfifo_init(__kfifo, buffer, size, sizeof(*__tmp->type)) : \
	-EINVAL; \
})

/**
 * kfifo_put - put data into the fifo
 * @fifo: address of the fifo to be used
 * @val: the data to be added 仅插入一个元素
 *
 * This macro copies the given value into the fifo.
 * It returns 0 if the fifo was full. Otherwise it returns the number
 * processed elements.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these macro.
 * // 插入函数：一个写者，可以不加自旋锁
 */
#define	kfifo_put(fifo, val) \
({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	typeof(*__tmp->const_type) __val = (val); \
	unsigned int __ret; \
	size_t __recsize = sizeof(*__tmp->rectype); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	if (__recsize) \
		__ret = __kfifo_in_r(__kfifo, &__val, sizeof(__val), \
			__recsize); \
	else { \
		__ret = !kfifo_is_full(__tmp); \
		if (__ret) { \
			(__is_kfifo_ptr(__tmp) ? \
				((typeof(__tmp->type))__kfifo->data) : \
				(__tmp->buf) \
			)[__kfifo->in & __tmp->kfifo.mask] = \
				(typeof(*__tmp->type))__val; \
			WMB(); \
			__kfifo->in++; \
		} \
	} \
	__ret; \
})

/**
 * kfifo_get - get data from the fifo
 * @fifo: address of the fifo to be used
 * @val: address where to store the data
 *
 * This macro reads the data from the fifo.
 * It returns 0 if the fifo was empty. Otherwise it returns the number
 * processed elements.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these macro.
 */
#define	kfifo_get(fifo, val) \
__kfifo_uint_must_check_helper( \
({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	typeof(__tmp->ptr) __val = (val); \
	unsigned int __ret; \
	const size_t __recsize = sizeof(*__tmp->rectype); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	if (__recsize) \
		__ret = __kfifo_out_r(__kfifo, __val, sizeof(*__val), \
			__recsize); \
	else { \
		__ret = !kfifo_is_empty(__tmp); \
		if (__ret) { \
			*(typeof(__tmp->type))__val = \
				(__is_kfifo_ptr(__tmp) ? \
				((typeof(__tmp->type))__kfifo->data) : \
				(__tmp->buf) \
				)[__kfifo->out & __tmp->kfifo.mask]; \
			WMB(); \
			__kfifo->out++; \
		} \
	} \
	__ret; \
}) \
)

/**
 * kfifo_peek - get data from the fifo without removing
 * @fifo: address of the fifo to be used
 * @val: address where to store the data
 *
 * This reads the data from the fifo without removing it from the fifo.
 * It returns 0 if the fifo was empty. Otherwise it returns the number
 * processed elements.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these macro.
 */
#define	kfifo_peek(fifo, val) \
__kfifo_uint_must_check_helper( \
({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	typeof(__tmp->ptr) __val = (val); \
	unsigned int __ret; \
	const size_t __recsize = sizeof(*__tmp->rectype); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	if (__recsize) \
		__ret = __kfifo_out_peek_r(__kfifo, __val, sizeof(*__val), \
			__recsize); \
	else { \
		__ret = !kfifo_is_empty(__tmp); \
		if (__ret) { \
			*(typeof(__tmp->type))__val = \
				(__is_kfifo_ptr(__tmp) ? \
				((typeof(__tmp->type))__kfifo->data) : \
				(__tmp->buf) \
				)[__kfifo->out & __tmp->kfifo.mask]; \
			WMB(); \
		} \
	} \
	__ret; \
}) \
)

/**
 * kfifo_in - put data into the fifo
 * @fifo: address of the fifo to be used
 * @buf: the data to be added
 * @n: number of elements to be added
 *
 * This macro copies the given buffer into the fifo and returns the
 * number of copied elements.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these macro.
 */
#define	kfifo_in(fifo, buf, n) \
({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	typeof(__tmp->ptr_const) __buf = (buf); \
	unsigned long __n = (n); \
	const size_t __recsize = sizeof(*__tmp->rectype); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	(__recsize) ?\
	__kfifo_in_r(__kfifo, __buf, __n, __recsize) : \
	__kfifo_in(__kfifo, __buf, __n); \
})

/**
 * kfifo_in_spinlocked - put data into the fifo using a spinlock for locking
 * @fifo: address of the fifo to be used
 * @buf: the data to be added
 * @n: number of elements to be added
 * @lock: pointer to the spinlock to use for locking
 *
 * This macro copies the given values buffer into the fifo and returns the
 * number of copied elements.
 */
#define	kfifo_in_spinlocked(fifo, buf, n, spin_lock_ptr) \
({ \
	unsigned int __ret; \
	spin_lock_ptr->lock(); \
	__ret = kfifo_in(fifo, buf, n); \
	spin_lock_ptr->unlock(); \
	__ret; \
})

/* alias for kfifo_in_spinlocked, will be removed in a future release */
#define kfifo_in_locked(fifo, buf, n, lock) \
		kfifo_in_spinlocked(fifo, buf, n, lock)

/**
 * kfifo_out - get data from the fifo
 * @fifo: address of the fifo to be used
 * @buf: pointer to the storage buffer
 * @n: max. number of elements to get
 *
 * This macro get some data from the fifo and return the numbers of elements
 * copied.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these macro.
 */
#define	kfifo_out(fifo, buf, n) \
__kfifo_uint_must_check_helper( \
({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	typeof(__tmp->ptr) __buf = (buf); \
	unsigned long __n = (n); \
	const size_t __recsize = sizeof(*__tmp->rectype); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	(__recsize) ?\
	__kfifo_out_r(__kfifo, __buf, __n, __recsize) : \
	__kfifo_out(__kfifo, __buf, __n); \
}) \
)

/**
 * kfifo_out_spinlocked - get data from the fifo using a spinlock for locking
 * @fifo: address of the fifo to be used
 * @buf: pointer to the storage buffer
 * @n: max. number of elements to get
 * @lock: pointer to the spinlock to use for locking
 *
 * This macro get the data from the fifo and return the numbers of elements
 * copied.
 */
#define	kfifo_out_spinlocked(fifo, buf, n, spin_lock_ptr) \
__kfifo_uint_must_check_helper( \
({ \
	unsigned int __ret; \
	spin_lock_ptr->lock(); \
	__ret = kfifo_out(fifo, buf, n); \
	spin_lock_ptr->unlock(); \
	__ret; \
}) \
)

/* alias for kfifo_out_spinlocked, will be removed in a future release */
#define kfifo_out_locked(fifo, buf, n, lock) \
		kfifo_out_spinlocked(fifo, buf, n, lock)

#if 0
/** 废弃
 * kfifo_from_user - puts some data from user space into the fifo
 * @fifo: address of the fifo to be used
 * @from: pointer to the data to be added
 * @len: the length of the data to be added
 * @copied: pointer to output variable to store the number of copied bytes
 * 			完成拷贝的字节数，用于返回
 *
 * This macro copies at most @len bytes from the @from into the
 * fifo, depending of the available space and returns -EFAULT/0.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these macro.
 */
#define	kfifo_from_user(fifo, from, len, copied) \
__kfifo_uint_must_check_helper( \
({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	const void __user *__from = (from); \
	unsigned int __len = (len); \
	unsigned int *__copied = (copied); \
	const size_t __recsize = sizeof(*__tmp->rectype); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	(__recsize) ? \
	__kfifo_from_user_r(__kfifo, __from, __len,  __copied, __recsize) : \
	__kfifo_from_user(__kfifo, __from, __len, __copied); \
}) \
)

/**
 * kfifo_to_user - copies data from the fifo into user space
 * @fifo: address of the fifo to be used
 * @to: where the data must be copied
 * @len: the size of the destination buffer
 * @copied: pointer to output variable to store the number of copied bytes
 *
 * This macro copies at most @len bytes from the fifo into the
 * @to buffer and returns -EFAULT/0.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these macro.
 */
#define	kfifo_to_user(fifo, to, len, copied) \
__kfifo_uint_must_check_helper( \
({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	void __user *__to = (to); \
	unsigned int __len = (len); \
	unsigned int *__copied = (copied); \
	const size_t __recsize = sizeof(*__tmp->rectype); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	(__recsize) ? \
	__kfifo_to_user_r(__kfifo, __to, __len, __copied, __recsize) : \
	__kfifo_to_user(__kfifo, __to, __len, __copied); \
}) \
)

/**
 * kfifo_dma_in_prepare - setup a scatterlist for DMA input
 * @fifo: address of the fifo to be used
 * @sgl: pointer to the scatterlist array
 * @nents: number of entries in the scatterlist array
 * @len: number of elements to transfer
 *
 * This macro fills a scatterlist for DMA input.
 * It returns the number entries in the scatterlist array.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these macros.
 */
#define	kfifo_dma_in_prepare(fifo, sgl, nents, len) \
({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	struct scatterlist *__sgl = (sgl); \
	int __nents = (nents); \
	unsigned int __len = (len); \
	const size_t __recsize = sizeof(*__tmp->rectype); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	(__recsize) ? \
	__kfifo_dma_in_prepare_r(__kfifo, __sgl, __nents, __len, __recsize) : \
	__kfifo_dma_in_prepare(__kfifo, __sgl, __nents, __len); \
})

/** dma 直接存储访问
 * kfifo_dma_in_finish - finish a DMA IN operation
 * @fifo: address of the fifo to be used
 * @len: number of bytes to received
 *
 * This macro finish a DMA IN operation. The in counter will be updated by
 * the len parameter. No error checking will be done.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these macros.
 */
#define kfifo_dma_in_finish(fifo, len) \
(void)({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	unsigned int __len = (len); \
	const size_t __recsize = sizeof(*__tmp->rectype); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	if (__recsize) \
		__kfifo_dma_in_finish_r(__kfifo, __len, __recsize); \
	else \
		__kfifo->in += __len / sizeof(*__tmp->type); \
})

/**
 * kfifo_dma_out_prepare - setup a scatterlist for DMA output
 * @fifo: address of the fifo to be used
 * @sgl: pointer to the scatterlist array
 * @nents: number of entries in the scatterlist array
 * @len: number of elements to transfer
 *
 * This macro fills a scatterlist for DMA output which at most @len bytes
 * to transfer.
 * It returns the number entries in the scatterlist array.
 * A zero means there is no space available and the scatterlist is not filled.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these macros.
 */
// #define	kfifo_dma_out_prepare(fifo, sgl, nents, len) \
// ({ \
// 	typeof((fifo) + 1) __tmp = (fifo);  \
// 	struct scatterlist *__sgl = (sgl); \
// 	int __nents = (nents); \
// 	unsigned int __len = (len); \
// 	const size_t __recsize = sizeof(*__tmp->rectype); \
// 	struct __kfifo *__kfifo = &__tmp->kfifo; \
// 	(__recsize) ? \
// 	__kfifo_dma_out_prepare_r(__kfifo, __sgl, __nents, __len, __recsize) : \
// 	__kfifo_dma_out_prepare(__kfifo, __sgl, __nents, __len); \
// })

/**
 * kfifo_dma_out_finish - finish a DMA OUT operation
 * @fifo: address of the fifo to be used
 * @len: number of bytes transferred
 *
 * This macro finish a DMA OUT operation. The out counter will be updated by
 * the len parameter. No error checking will be done.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these macros.
 */
#define kfifo_dma_out_finish(fifo, len) \
(void)({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	unsigned int __len = (len); \
	const size_t __recsize = sizeof(*__tmp->rectype); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	if (__recsize) \
		__kfifo_dma_out_finish_r(__kfifo, __recsize); \
	else \
		__kfifo->out += __len / sizeof(*__tmp->type); \
})

#endif

/** 仅从尾部获取一段数据
 * kfifo_out_peek - gets some data from the fifo
 * @fifo: address of the fifo to be used
 * @buf: pointer to the storage buffer
 * @n: max. number of elements to get
 *
 * This macro get the data from the fifo and return the numbers of elements
 * copied. The data is not removed from the fifo.
 *
 * Note that with only one concurrent reader and one concurrent
 * writer, you don't need extra locking to use these macro.
 */
#define	kfifo_out_peek(fifo, buf, n) \
__kfifo_uint_must_check_helper( \
({ \
	typeof((fifo) + 1) __tmp = (fifo); \
	typeof(__tmp->ptr) __buf = (buf); \
	unsigned long __n = (n); \
	const size_t __recsize = sizeof(*__tmp->rectype); \
	struct __kfifo *__kfifo = &__tmp->kfifo; \
	(__recsize) ? \
	__kfifo_out_peek_r(__kfifo, __buf, __n, __recsize) : \
	__kfifo_out_peek(__kfifo, __buf, __n); \
}) \
)

extern int __kfifo_alloc(struct __kfifo *fifo, unsigned int size,
	size_t esize);

extern void __kfifo_free(struct __kfifo *fifo);

extern int __kfifo_init(struct __kfifo *fifo, void *buffer,
	unsigned int size, size_t esize);

extern unsigned int __kfifo_in(struct __kfifo *fifo,
	const void *buf, unsigned int len);

extern unsigned int __kfifo_out(struct __kfifo *fifo,
	void *buf, unsigned int len);

// extern int __kfifo_from_user(struct __kfifo *fifo,
// 	const void __user *from, unsigned long len, unsigned int *copied);

// extern int __kfifo_to_user(struct __kfifo *fifo,
// 	void __user *to, unsigned long len, unsigned int *copied);

// extern unsigned int __kfifo_dma_in_prepare(struct __kfifo *fifo,
// 	struct scatterlist *sgl, int nents, unsigned int len);

// extern unsigned int __kfifo_dma_out_prepare(struct __kfifo *fifo,
// 	struct scatterlist *sgl, int nents, unsigned int len);

extern unsigned int __kfifo_out_peek(struct __kfifo *fifo,
	void *buf, unsigned int len);

extern unsigned int __kfifo_in_r(struct __kfifo *fifo,
	const void *buf, unsigned int len, size_t recsize);

extern unsigned int __kfifo_out_r(struct __kfifo *fifo,
	void *buf, unsigned int len, size_t recsize);

// extern int __kfifo_from_user_r(struct __kfifo *fifo,
// 	const void __user *from, unsigned long len, unsigned int *copied,
// 	size_t recsize);

// extern int __kfifo_to_user_r(struct __kfifo *fifo, void __user *to,
// 	unsigned long len, unsigned int *copied, size_t recsize);

// extern unsigned int __kfifo_dma_in_prepare_r(struct __kfifo *fifo,
// 	struct scatterlist *sgl, int nents, unsigned int len, size_t recsize);

// extern void __kfifo_dma_in_finish_r(struct __kfifo *fifo,
// 	unsigned int len, size_t recsize);

// extern unsigned int __kfifo_dma_out_prepare_r(struct __kfifo *fifo,
// 	struct scatterlist *sgl, int nents, unsigned int len, size_t recsize);

// extern void __kfifo_dma_out_finish_r(struct __kfifo *fifo, size_t recsize);

extern unsigned int __kfifo_len_r(struct __kfifo *fifo, size_t recsize);

extern void __kfifo_skip_r(struct __kfifo *fifo, size_t recsize);

extern unsigned int __kfifo_out_peek_r(struct __kfifo *fifo,
	void *buf, unsigned int len, size_t recsize);

extern unsigned int __kfifo_max_r(unsigned int len, size_t recsize);

#endif
