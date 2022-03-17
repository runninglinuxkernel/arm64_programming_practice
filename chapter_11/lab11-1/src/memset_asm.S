
.global __memset_16bytes
__memset_16bytes:
	mov x4, #0
1:
	stp x1, x1, [x0], #16
	add x4, x4, #16
	cmp x4, x2
	bne 1b
	ret
