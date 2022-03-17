.section .data
.align 3
.global my_atomic_data
my_atomic_data:
	.dword 0x0

.section .text
.global atomic_set
atomic_set:
1:
	ldxr x2, [x1]
	orr x2, x2, x0
	stxr w3, x2, [x1]
	cbnz w3, 1b
	ret	

.global my_atomic_write
my_atomic_write:
	adr x6, my_atomic_data
1:
	ldxr x2, [x6]
	orr x2, x2, x0
	stxr w3, x2, [x6]
	cbnz w3, 1b

	mov x0, x2
	ret
	
