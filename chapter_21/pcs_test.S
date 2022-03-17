	.arch armv8-a
	.section	.rodata
	.align	3
string:
	.string	"data: %d %d %d %d %d %d %d %d %d %d\n"

data:
	.word 1, 2, 3, 4, 5, 6, 7, 8, 9, -1

	.text
	.align	2

.global	main
main:
        /*栈往下扩展16字节。把x29保存到SP位置上，x30保存到SP+8上*/
	stp x29, x30, [sp, #-16]!

	/*读取data的地址*/
	adr x13, data 

	ldr w1, [x13, #(4*0)] // w1 = a
	ldr w2, [x13, #(4*1)] // w2 = b
	ldr w3, [x13, #(4*2)] // w3 = c
	ldr w4, [x13, #(4*3)] // w4 = d
	ldr w5, [x13, #(4*4)] // w5 = e
	ldr w6, [x13, #(4*5)] // w6 = f
	ldr w7, [x13, #(4*6)] // w7 = g

	ldr w8, [x13, #(4*7)] // w8 = h
	ldr w9, [x13, #(4*8)] // w9 = i
	ldr w10, [x13, #(4*9)] // w10 = j

	/*把栈 继续往下扩大32字节*/
	add sp, sp, #-32

	/* 把w8~w10这三个参数保存到栈里，w8保存到SP位置上
	  W9保存到SP+8，W10保存到SP+16
	 */
	str w10, [sp, #16]
	str w9, [sp, #8]
	str w8, [sp]

	/*printf*/
	adrp	x0, string
	add	x0, x0, :lo12:string
	bl	printf

	/*释放刚才扩展的32字节的栈空间*/
	add sp, sp, #32

	mov	w0, 0
	/*恢复x29和x30，然后SP回到原点*/
	ldp x29, x30, [sp], #16
	ret
