#include <stdio.h>

static inline void my_atomic_add(unsigned long val, void *p)
{
	unsigned long tmp;
	int result;
	asm volatile (
			"1: ldxr %0, %2\n"
			"add %0, %0, %3\n"
			"stxr %w1, %0, %2\n"
			"cbnz %w1, 1b\n"
			: "+r" (tmp), "+r"(result), "+Q"(*(unsigned long *)p)
			: "r" (val)
			: "cc"
		     );
}

int main(void)
{
	unsigned long p = 0;

	my_atomic_add(2, &p);

	printf("atomic add: counter=%ld\n", p);

}

