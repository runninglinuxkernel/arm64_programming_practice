.align 3
.global my_data
my_data:
	.dword 0x0

.global my_atomic_write
my_atomic_write:
	adr x6, my_data
1:
	ldxr x2, [x6]
	mov  x2, x0
	stxr w3, x2, [x6]
	cbnz w3, 1b

	mov x0, x2
	ret
