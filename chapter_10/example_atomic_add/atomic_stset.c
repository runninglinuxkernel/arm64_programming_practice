#include <stdio.h>

__asm__ (".arch_extension lse");

void
foo (int i, int *v)
{
  register int w0 asm ("w0") = i;
  register int *x1 asm ("x1") = v;

  asm volatile (
  "stset   %w[i], %[v]\n"
  : [i] "+r" (w0), [v] "+Q" (*v)
  : "r" (x1)
  : "x30");
}

int main(void)
{
	int p = 0x100;

	foo(3, &p);

	printf("atomic : counter=0x%x\n", p);

}

