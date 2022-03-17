
.global rgb24_bgr24_asm
rgb24_bgr24_asm:
		mov x3, #0
		whilelo p0.b, x3, x2

		1:
		ld3b    {z4.b - z6.b}, p0/z, [x0, x3]
		mov     z1.d, z6.d
		mov     z2.d, z5.d
		mov     z3.d, z4.d
		st3b    {z1.b - z3.b}, p0, [x1, x3]
		incb    x3, all, mul #3
		whilelo p0.b, x3, x2
		b.any 1b

		ret
