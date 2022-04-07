#include <stdio.h>

/*
 * 它的目的是打印出test()函数的符号地址。
 *
 * 内嵌汇编代码翻译之后变成这样：
     .pushsection ".data","aw"
     .align 3
     my_test:
	  .quad test
     .popsection
     ldr x0, my_test
 *
 * 编译程序并运行测试。(需要在QEMU+ARM64 Linux系统中)
   # gcc asm_inline_c.c -o asm_inline_c
   # ./asm_inline_c
 */

void test(void)
{
	unsigned long addr;
	asm volatile (
			".pushsection \".data\",\"aw\"\n"
			".align 3\n"
			"my_test:\n"
			".quad %c1\n"
			".popsection\n"
			"ldr %0, my_test\n"
			: "=r" (addr)
			: "S" (&test) 
			: "memory"
		     );

	printf("addr = 0x%lx \n", addr);
	printf("test function addr = 0x%lx \n", &test);
}

int main(void)
{
	test();
}

