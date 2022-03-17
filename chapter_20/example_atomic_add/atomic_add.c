#include <stdio.h>

typedef struct {
	int counter;
} atomic_t;

#if 1
static inline void atomic_add(int i, atomic_t *v)
{
	unsigned long tmp;
	int result;

	asm volatile("// atomic_add\n"
"1:	ldxr	%w0, [%2]\n"
"	add	%w0, %w0, %w3\n"
"	stxr	%w1, %w0, [%2]\n"
"	cbnz	%w1, 1b"
	: "=&r" (result), "=&r" (tmp)
	: "r" (&v->counter), "Ir" (i)
	:"cc", "memory");
}
#else
static inline void atomic_add(int i, atomic_t *v)
{
	unsigned long tmp;
	int result;

	asm volatile("// atomic_add\n"
"1:	ldxr	%w0, %2\n"
"	add	%w0, %w0, %w3\n"
"	stxr	%w1, %w0, %2\n"
"	cbnz	%w1, 1b"
	: "=&r" (result), "=&r" (tmp), "+Q" (v->counter)
	: "Ir" (i)
	: "cc");
}
#endif

int main(void)
{
	atomic_t v;
	v.counter = 0;

	atomic_add(2, &v);

	printf("atomic add: counter=%d\n", v.counter);
}

