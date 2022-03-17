#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <arm_neon.h>
#include <time.h>

static void rgb24_bgr24_c(unsigned char *src, unsigned char *dst, unsigned long count)
{
	unsigned long i;

	for (i = 0; i < count; i++) {
		dst[3 * i] = src[3 * i +2];
		dst[3 * i + 1] = src[3*i + 1];
		dst[3 * i + 2] = src[3*i];
	}
}

static void rgb24_bgr24_asm(unsigned char *src, unsigned char *dst, unsigned long count)
{
	count = count * 3;
	unsigned long size = 0;

	asm volatile (
		"1: ld3 {v0.16b, v1.16b, v2.16b}, [%[src]], #48 \n"
		"mov v3.16b, v0.16b\n"
		"mov v0.16b, v2.16b\n"
		"mov v2.16b, v3.16b\n"
		"st3 {v0.16b, v1.16b, v2.16b}, [%[dst]], #48\n"
		"add %[size], %[size], #48\n"
		"cmp %[size], %[count]\n"
		"bne 1b\n"
		: [dst] "+r"(dst), [src] "+r"(src), [size] "+r"(size)
		: [count] "r" (count)
		: "memory", "v0", "v1", "v2", "v3"
		);
}

static void rgb24_bgr24_neon_intr(unsigned char *src, unsigned char *dst, unsigned long count)
{
	unsigned long i;

	count = count * 3;

	uint8x16x3_t rgb;
	uint8x16x3_t bgr;

	uint8x16_t vzero = vmovq_n_u8(0);

	for (i = 0; i < count/48; i++) {
		rgb = vld3q_u8(src +3*16*i);

#if 0
		bgr.val[2] = rgb.val[0];
		bgr.val[0] = rgb.val[2];
		bgr.val[1] = rgb.val[1];
	
		vst3q_u8(dst + 3 * 16 * i, bgr);
#else
		bgr.val[2] = vorrq_u8(rgb.val[0], vzero);
		bgr.val[0] = vorrq_u8(rgb.val[2], vzero);
		bgr.val[1] = vorrq_u8(rgb.val[1], vzero);

		vst3q_u8(dst + 3 * 16 * i, bgr);
#endif
	}
}

#define IMAGE_SIZE (4096 * 2160 *10)
#define PIXEL_SIZE (IMAGE_SIZE * 3)

int main(int argc, char* argv[])
{
	unsigned long i;
	struct timespec time_start, time_end;
	unsigned long time_c, time_neon, time_asm;

	unsigned char *rgb24_src = malloc(PIXEL_SIZE);
	if (!rgb24_src)
		return 0;
	memset(rgb24_src, 0, PIXEL_SIZE);

	unsigned char *bgr24_c = malloc(PIXEL_SIZE);
	if (!bgr24_c)
		return 0;
	memset(bgr24_c, 0, PIXEL_SIZE);

	unsigned char *bgr24_asm = malloc(PIXEL_SIZE);
	if (!bgr24_asm)
		return 0;
	memset(bgr24_asm, 0, PIXEL_SIZE);

	unsigned char *bgr24_neon = malloc(PIXEL_SIZE);
	if (!bgr24_neon)
		return 0;
	memset(bgr24_neon, 0, PIXEL_SIZE);

	for (i = 0; i < PIXEL_SIZE; i++) {
		rgb24_src[i] = rand() & 0xff;
	}
	
	clock_gettime(CLOCK_REALTIME,&time_start);
        rgb24_bgr24_c(rgb24_src, bgr24_c, IMAGE_SIZE);	
	clock_gettime(CLOCK_REALTIME,&time_end);
	time_c = (time_end.tv_sec - time_start.tv_sec)*1000 +
		(time_end.tv_nsec - time_start.tv_nsec)/1000000;
	printf("c spend time :%ld ms\n", time_c);

	clock_gettime(CLOCK_REALTIME,&time_start);
	rgb24_bgr24_neon_intr(rgb24_src, bgr24_neon, IMAGE_SIZE);
	clock_gettime(CLOCK_REALTIME,&time_end);
	time_neon = (time_end.tv_sec - time_start.tv_sec)*1000 +
		(time_end.tv_nsec - time_start.tv_nsec)/1000000;
	printf("neon intr spend time :%ld ms\n", time_neon);

	clock_gettime(CLOCK_REALTIME,&time_start);
	rgb24_bgr24_asm(rgb24_src, bgr24_asm, IMAGE_SIZE);
	clock_gettime(CLOCK_REALTIME,&time_end);
	time_asm = (time_end.tv_sec - time_start.tv_sec)*1000 +
		(time_end.tv_nsec - time_start.tv_nsec)/1000000;
	printf("asm spend time :%ld ms\n", time_asm);

	if (memcmp(bgr24_c, bgr24_asm, PIXEL_SIZE) || memcmp(bgr24_c, bgr24_neon, PIXEL_SIZE))
		printf("error on bgr data\n");
	else
		printf("bgr result (%ld) is idential\n", PIXEL_SIZE);

	printf("asm fast than c: %f\n", (float)time_c/time_asm);
	printf("asm fast than neon_intr: %f\n", (float)time_neon/time_asm);

	free(rgb24_src);
	free(bgr24_c);
	free(bgr24_asm);
	free(bgr24_neon);

	return 0;
}
