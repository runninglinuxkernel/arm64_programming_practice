#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include <arm_neon.h>

#define LOOP 10000

static void matrix_multiply_c(float32_t *A, float32_t *B, float32_t *C)
{
        for (int i_idx=0; i_idx<4; i_idx++) {
                for (int j_idx=0; j_idx<4; j_idx++) {
                        C[4*j_idx + i_idx] = 0;
                        for (int k_idx=0; k_idx<4; k_idx++) {
                                C[4*j_idx + i_idx] += A[4*k_idx + i_idx]*B[4*j_idx + k_idx];
                        }
                }
        }
}

static void matrix_multiply_4x4_neon(float32_t *A, float32_t *B, float32_t *C)
{
	/*矩阵A分成4列：A0，A1，A2，A3 */
        float32x4_t A0;
        float32x4_t A1;
        float32x4_t A2;
        float32x4_t A3;
        
	/*矩阵B分成4列：B0，B1，B2，B3 */
        float32x4_t B0;
        float32x4_t B1;
        float32x4_t B2;
        float32x4_t B3;
        
	/*矩阵C分成4列：C0，C1，C2，C3 */
        float32x4_t C0;
        float32x4_t C1;
        float32x4_t C2;
        float32x4_t C3;
        
        A0 = vld1q_f32(A);
        A1 = vld1q_f32(A+4);
        A2 = vld1q_f32(A+8);
        A3 = vld1q_f32(A+12);
        
        C0 = vmovq_n_f32(0);
        C1 = vmovq_n_f32(0);
        C2 = vmovq_n_f32(0);
        C3 = vmovq_n_f32(0);
        
	/*计算C0: 第0列*/
        B0 = vld1q_f32(B);
        C0 = vfmaq_laneq_f32(C0, A0, B0, 0);
        C0 = vfmaq_laneq_f32(C0, A1, B0, 1);
        C0 = vfmaq_laneq_f32(C0, A2, B0, 2);
        C0 = vfmaq_laneq_f32(C0, A3, B0, 3);
        vst1q_f32(C, C0);
        
	/*计算C1: 第1列*/
        B1 = vld1q_f32(B+4);
        C1 = vfmaq_laneq_f32(C1, A0, B1, 0);
        C1 = vfmaq_laneq_f32(C1, A1, B1, 1);
        C1 = vfmaq_laneq_f32(C1, A2, B1, 2);
        C1 = vfmaq_laneq_f32(C1, A3, B1, 3);
        vst1q_f32(C+4, C1);
        
	/*计算C2: 第2列*/
        B2 = vld1q_f32(B+8);
        C2 = vfmaq_laneq_f32(C2, A0, B2, 0);
        C2 = vfmaq_laneq_f32(C2, A1, B2, 1);
        C2 = vfmaq_laneq_f32(C2, A2, B2, 2);
        C2 = vfmaq_laneq_f32(C2, A3, B2, 3);
        vst1q_f32(C+8, C2);
        
	/*计算C3: 第3列*/
        B3 = vld1q_f32(B+12);
        C3 = vfmaq_laneq_f32(C3, A0, B3, 0);
        C3 = vfmaq_laneq_f32(C3, A1, B3, 1);
        C3 = vfmaq_laneq_f32(C3, A2, B3, 2);
        C3 = vfmaq_laneq_f32(C3, A3, B3, 3);
        vst1q_f32(C+12, C3);
}

void matrix_multiply_4x4_asm(float32_t *A, float32_t *B, float32_t *C)
{
	asm volatile (
			"ld1 {v0.4s, v1.4s, v2.4s, v3.4s}, [%[a]]\n"
			"ld1 {v4.4s, v5.4s, v6.4s, v7.4s}, [%[b]]\n"

			"movi	v8.4s, 0\n"
			"movi	v9.4s, 0\n"
			"movi	v10.4s, 0\n"
			"movi	v11.4s, 0\n"
			
			/*计算C0: 第0列*/
			"fmla v8.4s, v0.4s, v4.s[0]\n"
			"fmla v8.4s, v1.4s, v4.s[1]\n"
			"fmla v8.4s, v2.4s, v4.s[2]\n"
			"fmla v8.4s, v3.4s, v4.s[3]\n"

			/*计算C1: 第1列*/
			"fmla v9.4s, v0.4s, v5.s[0]\n"
			"fmla v9.4s, v1.4s, v5.s[1]\n"
			"fmla v9.4s, v2.4s, v5.s[2]\n"
			"fmla v9.4s, v3.4s, v5.s[3]\n"

			/*计算C2: 第2列*/
			"fmla v10.4s, v0.4s, v6.s[0]\n"
			"fmla v10.4s, v1.4s, v6.s[1]\n"
			"fmla v10.4s, v2.4s, v6.s[2]\n"
			"fmla v10.4s, v3.4s, v6.s[3]\n"

			/*计算C3: 第3列*/
			"fmla v11.4s, v0.4s, v7.s[0]\n"
			"fmla v11.4s, v1.4s, v7.s[1]\n"
			"fmla v11.4s, v2.4s, v7.s[2]\n"
			"fmla v11.4s, v3.4s, v7.s[3]\n"

			"st1 {v8.4s, v9.4s, v10.4s, v11.4s}, [%[c]]\n"
			:
			: [a] "r" (A), [b] "r" (B), [c] "r" (C)
			: "memory", "v0", "v1", "v2", "v3",
				"v4", "v5", "v6", "v7", "v8",
				"v9", "v10", "v11"
		     );
}

static void print_matrix(float32_t *M, uint32_t cols, uint32_t rows)
{
        for (int i=0; i<rows; i++) {
                for (int j=0; j<cols; j++) {
                        printf("%f ", M[j*rows + i]);
                }
                printf("\n");
        }
        printf("\n");
}

static void matrix_init_rand(float32_t *M, uint32_t numvals)
{
        for (int i=0; i<numvals; i++) {
                M[i] = (float)rand()/(float)(RAND_MAX);
        }
}

static void matrix_init(float32_t *M, uint32_t cols, uint32_t rows, float32_t val)
{
        for (int i=0; i<rows; i++) {
                for (int j=0; j<cols; j++) {
                        M[j*rows + i] = val;
                }
        }
}

static bool f32comp_noteq(float32_t a, float32_t b)
{
        if (fabs(a-b) < 0.000001) {
                return false;
        }
        return true;
}

static bool matrix_comp(float32_t *A, float32_t *B, uint32_t rows, uint32_t cols)
{
        float32_t a;
        float32_t b;
        for (int i=0; i<rows; i++) {
                for (int j=0; j<cols; j++) {
                        a = A[rows*j + i];
                        b = B[rows*j + i];      
                        
                        if (f32comp_noteq(a, b)) {
                                printf("i=%d, j=%d, A=%f, B=%f\n", i, j, a, b);
                                return false;
                        }
                }
        }
        return true;
}

int main()
{
	int i;
	struct timespec time_start, time_end;
	unsigned long clocks_c, clocks_neon, clocks_asm;
        
        float32_t A[16];
        float32_t B[16];
        float32_t C[16];
        float32_t D[16];
        float32_t E[16];
        
        bool c_eq_asm;
        bool c_eq_neon;

        matrix_init_rand(A, 16);
        matrix_init_rand(B, 16);
        matrix_init(C, 4, 4, 0);
	
	printf("Matrix A:\n");
	print_matrix(A, 4, 4);
	printf("Matrix B:\n");
	print_matrix(B, 4, 4);
        
	clock_gettime(CLOCK_REALTIME,&time_start);
	for (i = 0; i < LOOP; i++)
		matrix_multiply_c(A, B, C);
	clock_gettime(CLOCK_REALTIME,&time_end);
	clocks_c = (time_end.tv_sec - time_start.tv_sec)*1000000 +
		(time_end.tv_nsec - time_start.tv_nsec)/1000;
	printf("c spent time :%ld us\n", clocks_c);
        print_matrix(C, 4, 4);
        
	clock_gettime(CLOCK_REALTIME,&time_start);
	for (i = 0; i < LOOP; i++)
		matrix_multiply_4x4_neon(A, B, D);
	clock_gettime(CLOCK_REALTIME,&time_end);
	clocks_neon = (time_end.tv_sec - time_start.tv_sec)*1000000 +
		(time_end.tv_nsec - time_start.tv_nsec)/1000;
	printf("Neon Intrinsics spent time :%ld us\n", clocks_neon);
        print_matrix(D, 4, 4);

	clock_gettime(CLOCK_REALTIME,&time_start);
	for (i = 0; i < LOOP; i++)
		matrix_multiply_4x4_asm(A, B, E);
	clock_gettime(CLOCK_REALTIME,&time_end);
	clocks_asm = (time_end.tv_sec - time_start.tv_sec)*1000000 +
		(time_end.tv_nsec - time_start.tv_nsec)/1000;
	printf("asm spent time :%ld us\n", clocks_asm);
        print_matrix(E, 4, 4);
	
        c_eq_neon = matrix_comp(C, D, 4, 4);
        printf("Neon equal to C: %s\n", c_eq_neon ? "yes":"no");
        c_eq_neon = matrix_comp(C, E, 4, 4);
        printf("Asm equal to C:  %s\n", c_eq_neon ? "yes" : "no");
        printf("===============================\n");

	printf("asm faster than c: %f\n", (float)clocks_c/clocks_asm);
	printf("asm faster than neon Intrinsics: %f\n", (float)clocks_neon/clocks_asm);
}
