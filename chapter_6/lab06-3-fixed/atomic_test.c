#include <stdio.h>

extern unsigned long my_atomic_write(long a);

int main()
{
	unsigned long p1 = 0;
	printf("p1 address: 0x%lx\n", &p1);

	p1 = my_atomic_write(0x34);

	printf("after atomic write: 0x%x\n", p1);

	return 0;

}
