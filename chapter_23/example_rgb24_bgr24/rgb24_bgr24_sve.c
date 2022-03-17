#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

static void rgb24_bgr24_c(unsigned char *src, unsigned char *dst, unsigned long count)
{
	unsigned long i;

	for (i = 0; i < count/3; i++) {
		dst[3 * i] = src[3 * i +2];
		dst[3 * i + 1] = src[3*i + 1];
		dst[3 * i + 2] = src[3*i];
	}
}

static void rgb24_bgr24_inline_asm(unsigned char *src, unsigned char *dst, unsigned long count)
{
	unsigned long size = 0;

	asm volatile (
		"mov x4, %[count]\n"
		"mov x2, #0\n"
		"whilelo p0.b, x2, x4\n"

		"1: ld3b    {z4.b - z6.b}, p0/z, [%[src], x2]\n"
		"mov     z1.d, z6.d\n"
		"mov     z2.d, z5.d\n"
		"mov     z3.d, z4.d\n"
		"st3b    {z1.b - z3.b}, p0, [%[dst], x2]\n"
		"incb    x2, all, mul #3\n"
		"WHILELT p0.b, x2, x4\n"
		"b.any 1b\n"
		: [dst] "+r"(dst), [src] "+r"(src), [size] "+r"(size)
		: [count] "r" (count)
		: "memory", "z1", "z2", "z3", "z4","z5","z6","x4", "x2","p0"
		);
}

#define IMAGE_SIZE (4096 * 2160)
#define PIXEL_SIZE (IMAGE_SIZE * 3)

int main(int argc, char* argv[])
{
	unsigned long i;

	unsigned char *rgb24_src = malloc(PIXEL_SIZE);
	if (!rgb24_src)
		return 0;
	memset(rgb24_src, 0, PIXEL_SIZE);

	unsigned char *bgr24_c = malloc(PIXEL_SIZE);
	if (!bgr24_c)
		return 0;
	memset(bgr24_c, 0, PIXEL_SIZE);

	unsigned char *bgr24_inline_asm = malloc(PIXEL_SIZE);
	if (!bgr24_inline_asm)
		return 0;
	memset(bgr24_inline_asm, 0, PIXEL_SIZE);

	unsigned char *bgr24_asm = malloc(PIXEL_SIZE);
	if (!bgr24_asm)
		return 0;
	memset(bgr24_asm, 0, PIXEL_SIZE);

	for (i = 0; i < PIXEL_SIZE; i++) {
		rgb24_src[i] = rand() & 0xff;
	}
	
        rgb24_bgr24_c(rgb24_src, bgr24_c, PIXEL_SIZE);	

	rgb24_bgr24_inline_asm(rgb24_src, bgr24_inline_asm, PIXEL_SIZE);

	rgb24_bgr24_asm(rgb24_src, bgr24_asm, PIXEL_SIZE);

	if (memcmp(bgr24_c, bgr24_inline_asm, PIXEL_SIZE))
		printf("error on bgr24_inline_asm data\n");
	else
		printf("bgr24_c (%ld) is idential with bgr24_inline_asm\n", PIXEL_SIZE);

	if (memcmp(bgr24_c, bgr24_asm, PIXEL_SIZE))
		printf("error on bgr24_asm data\n");
	else
		printf("bgr24_c (%ld) is idential with bgr24_asm\n", PIXEL_SIZE);

	free(rgb24_src);
	free(bgr24_c);
	free(bgr24_inline_asm);
	free(bgr24_asm);

	return 0;
}
