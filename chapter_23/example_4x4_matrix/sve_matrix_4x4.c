#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef float float32_t;

#define BLOCK_SIZE 4
#define LOOP 10000

static void matrix_multiply_c(float32_t *A, float32_t *B, float32_t *C, uint32_t n, uint32_t m, uint32_t k)
{
        for (int i_idx=0; i_idx<n; i_idx++) {
                for (int j_idx=0; j_idx<m; j_idx++) {
                        C[n*j_idx + i_idx] = 0;
                        for (int k_idx=0; k_idx<k; k_idx++) {
                                C[n*j_idx + i_idx] += A[n*k_idx + i_idx]*B[k*j_idx + k_idx];
                        }
                }
        }
}

void matrix_multiply_4x4_asm(float32_t *A, float32_t *B, float32_t *C)
{
	asm volatile (
			"ptrue p0.s,vl4\n"

			/*加载A矩阵的数据到Z0~Z3*/
			"ld1w {z0.s}, p0/z, [%[a]]\n"
			"incw %[a], VL4, MUL #4\n"
			"ld1w {z1.s}, p0/z, [%[a]]\n"
			"incw %[a], VL4, MUL #4\n"
			"ld1w {z2.s}, p0/z, [%[a]]\n"
			"incw %[a], VL4, MUL #4\n"
			"ld1w {z3.s}, p0/z, [%[a]]\n"

			/*加载B矩阵的数据到Z4~Z7*/
			"ld1w {z4.s}, p0/z, [%[b]]\n"
			"incw %[b], VL4, MUL #4\n"
			"ld1w {z5.s}, p0/z, [%[b]]\n"
			"incw %[b], VL4, MUL #4\n"
			"ld1w {z6.s}, p0/z, [%[b]]\n"
			"incw %[b], VL4, MUL #4\n"
			"ld1w {z7.s}, p0/z, [%[b]]\n"

			/*计算C0: 第0列*/
			"fmul z8.s, z0.s, z4.s[0]\n"
			"fmla z8.s, z1.s, z4.s[1]\n"
			"fmla z8.s, z2.s, z4.s[2]\n"
			"fmla z8.s, z3.s, z4.s[3]\n"

			/*计算C1: 第1列*/
			"fmul z9.s, z0.s, z5.s[0]\n"
			"fmla z9.s, z1.s, z5.s[1]\n"
			"fmla z9.s, z2.s, z5.s[2]\n"
			"fmla z9.s, z3.s, z5.s[3]\n"

			/*计算C2: 第2列*/
			"fmul z10.s, z0.s, z6.s[0]\n"
			"fmla z10.s, z1.s, z6.s[1]\n"
			"fmla z10.s, z2.s, z6.s[2]\n"
			"fmla z10.s, z3.s, z6.s[3]\n"

			/*计算C3: 第3列*/
			"fmul z11.s, z0.s, z7.s[0]\n"
			"fmla z11.s, z1.s, z7.s[1]\n"
			"fmla z11.s, z2.s, z7.s[2]\n"
			"fmla z11.s, z3.s, z7.s[3]\n"

			"st1w {z8.s}, p0, [%[c]]\n"
			"incw %[c], VL4, MUL #4\n"
			"st1w {z9.s}, p0, [%[c]]\n"
			"incw %[c], VL4, MUL #4\n"
			"st1w {z10.s}, p0, [%[c]]\n"
			"incw %[c], VL4, MUL #4\n"
			"st1w {z11.s}, p0, [%[c]]\n"
			:
			: [a] "r" (A), [b] "r" (B), [c] "r" (C)
			: "memory", "z0", "z1", "z2", "z3",
				"z4", "z5", "z6", "z7", "z8",
				"z9", "z10", "z11", "p0"
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
        uint32_t n = BLOCK_SIZE; // rows in A
        uint32_t m = BLOCK_SIZE; // cols in B
        uint32_t k = BLOCK_SIZE; // cols in a and rows in b
        
        float32_t A[n*k];
        float32_t B[k*m];
        float32_t C[n*m];
        float32_t D[n*m];
        
        bool c_eq_asm;
        bool c_eq_neon;

        matrix_init_rand(A, n*k);
        matrix_init_rand(B, k*m);
        matrix_init(C, n, m, 0);
	
	printf("A[] data:\n");
	print_matrix(A, m, k);
	
	printf("B[] data:\n");
	print_matrix(B, m, k);
        //print_matrix(C, n, m);
        
	for (i = 0; i < LOOP; i++)
		matrix_multiply_c(A, B, C, n, m, k);
        printf("C result:\n");
        print_matrix(C, n, m);
        
	for (i = 0; i < LOOP; i++)
		matrix_multiply_4x4_asm(A, B, D);
        printf("asm result:\n");
        print_matrix(D, n, m);
	
        c_eq_neon = matrix_comp(C, D, n, m);
        printf("Asm equal to C:  %s\n", c_eq_neon ? "yes" : "no");
        printf("===============================\n");

}
