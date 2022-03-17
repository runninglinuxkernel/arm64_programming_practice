#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static void rgb24_bgr24_c(unsigned char *src, unsigned char *dst, unsigned long count)
{
	unsigned char r, g, b;
	unsigned long i;

	for (i = 0; i < count; i++) {
		r = src[3 * i];
		g = src[3 * i + 1];
		b = src[3 * i +2];

		dst[3 * i] = b;
		dst[3 * i + 1] = g;
		dst[3 * i + 2] = r;
	}
}

#define IMAGE_SIZE (1920 * 1080)
#define PIXEL_SIZE (IMAGE_SIZE * 3)

int main(int argc, char* argv[])
{
	unsigned long i;
	unsigned long clocks_c, clocks_asm;

	unsigned char *rgb24_src = malloc(PIXEL_SIZE);
	unsigned char *bgr24_c = malloc(PIXEL_SIZE);

	clock_t start = clock();

        rgb24_bgr24_c(rgb24_src, bgr24_c, IMAGE_SIZE);	

	clock_t end = clock();
	clocks_c = end - start;
	printf("c spend time :%ld\n", clocks_c);

	return 0;
}
