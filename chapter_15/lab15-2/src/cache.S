
.global get_cache_line_size
get_cache_line_size:
	mrs x0, ctr_el0	
	ubfm x0, x0, #16, #19
	mov x1, #4
	lsl x0, x1, x0
	ret

/*
   flush_cache_range(start, end)
 */
.global flush_cache_range
flush_cache_range:
	mov x7, x30

	mov x8, x0
	mov x9, x1
	
	bl get_cache_line_size
	sub x3, x0, #1
	bic x4, x8, x3
1:
	dc civac, x4
	add x4, x4, x0
	cmp x4, x9
	b.lo	1b

	dsb	ish

	mov x30, x7
	ret

	

	
