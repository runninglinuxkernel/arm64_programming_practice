#include "mm.h"

.section  .rodata
.align 3
.globl el_string1
el_string1:
	.string "Booting at EL"

.section ".text.boot"
.globl _start
_start:
	mrs	x0, mpidr_el1		
	and	x0, x0,#0xFF		// Check processor id
	cbz	x0, master		// Hang for all non-primary CPU
	b	proc_hang

proc_hang: 
	b 	proc_hang

master:
	/*
	   假设代码段是在ROM中(LMA)，而ROM的地址在0x90000

	   我们需要把代码段 从加载地址(LMA)拷贝到 运行地址(VMA)
	 */
	adr x0, TEXT_ROM
	adr x1, _text
	adr x2, _etext
1:
	ldr x4, [x0], #8
	str x4, [x1], #8
	cmp x1, x2
	b.cc 1b

        /* init uart and print the string*/
	bl __init_uart
	bl print_el

	adr	x0, _bss
	adr	x1, _ebss
	sub	x1, x1, x0
	bl 	memzero


	mov	sp, #LOW_MEMORY 
	bl	kernel_main
	b 	proc_hang		// should never come here

print_el:
	mov x10, x30

	/*
	   print EL
	 */
	adrp x0, el_string1
	add x0, x0, :lo12:el_string1
	bl put_string_uart

	mrs x5, CurrentEL
	/* get the currentEL value */
	lsr x2, x5, #2
	mov x0, #48
	add x0, x0, x2
	bl put_uart
	/* print the new line tab */
	mov x0, #10
	bl put_uart

	mov x30, x10
	ret
