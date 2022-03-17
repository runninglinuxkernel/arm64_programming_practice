.global sve_ld1_test
sve_ld1_test:
	lsr  x2, x2, 2
	mov x3, #0
	whilelt p0.s, x3, x2

	1:
	ld1w {z0.s}, p0/z, [x1, x3, lsl 2]
	st1w {z0.s}, p0, [x0, x3, lsl 2]
	incw x3
	whilelt p0.s, x3, x2
	b.any 1b

	ret
