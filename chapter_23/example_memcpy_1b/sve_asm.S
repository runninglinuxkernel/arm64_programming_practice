
.global sve_ld1_test
sve_ld1_test:
	mov x3, #0
	whilelt p0.b, x3, x2

	1:
	ld1b {z0.b}, p0/z, [x1, x3]
	st1b {z0.b}, p0, [x0, x3]
	incb x3
	whilelt p0.b, x3, x2
	b.any 1b

	ret
