#include <stdio.h>
#include <stdlib.h>

#define SIZE 10

static void swap_data(unsigned char *src, unsigned char *dst,unsigned int size)
{
	unsigned int len = 0;
	unsigned int tmp;

	asm volatile (
		"1: ldrh w5, [%[src]], #2\n"
		"lsl w6, w5, #8\n"
		"orr %w[tmp], w6, w5, lsr #8\n"
		"strh %w[tmp], [%[dst]], #2\n"
		"add %[len], %[len], #2\n"
		"cmp %[len], %[size]\n"
		"bne 1b\n"
		: [dst] "+r" (dst), [len] "+r"(len), [tmp] "+r" (tmp)
		: [src] "r" (src), [size] "r" (size)
		: "memory"
	);
}

int main(void)
{
	int i;
	unsigned char *bufa = malloc(SIZE);
	if (!bufa)
		return 0;

	unsigned char *bufb = malloc(SIZE);
	if (!bufb) {
		free(bufa);
		return 0;
	}

	//printf("%p \n", bufa);

	for (i = 0; i < SIZE; i++) {
		bufa[i] = i;
		printf("%d ", bufa[i]);
	}
	printf("\n");
	
	swap_data(bufa, bufb, SIZE);
	//printf("%p \n", bufa);

	for (i = 0; i < SIZE; i++)
		printf("%d ", bufb[i]);
	printf("\n");

	free(bufa);
	free(bufb);
	
	return 0;
}
