#include "memset.h"

typedef unsigned int size_t;

static void *__memset_1bytes(void *s, int c, size_t count)
{
	char *xs = s;

	while (count--)
		*xs++ = c;
	return s;
}

static void *__memset(char *s, int c, size_t count)
{
	char *p = s;
	unsigned long align = 16;
	size_t size, left = count;
	int n, i;
	unsigned long address = (unsigned long)p;
	unsigned long data = 0ULL;

	/* 这里c必须转换成unsigned long类型
	 * 否则 只能设置4字节，因为c变量是int类型
	 */
	for (i = 0; i < 8; i++)
		data |= (((unsigned long)c) & 0xff) << (i * 8);

	/*1. check start address is align with 16 bytes */
	if (address & (align - 1)) {
		//fixme: 这里应该是 对齐的后半段
		size = align - address & (align - 1);
		__memset_1bytes(p, c, size);
		p = p + size;
		left = count - size;
	}

	/*align 16 bytes*/
	if (left > align) {
		n = left / align;
		left = left % align;

		__memset_16bytes(p, data, 16*n);

		if (left)
			__memset_1bytes(p + 16*n, c, left);
	}

	return s;
}

void *memset(void *s, int c, size_t count)
{
	return __memset(s, c, count);
}
