 # 测试程序：往终端中输出my_data1数据与my_data2数据之和
 .section .data
 .align	3
 
 my_data1:
 	.quad  100 //data
 
my_data2:
	.word  50

print_data:
	.string "data: %d\n"

.align	3
.section .text

.global main
main:
	stp     x29, x30, [sp, -16]!

	ldr x5, =my_data1
	ldr x2, [x5]

	ldr x6, =my_data2
	ldr x3, [x6]

	add x1, x2, x3

	ldr x0, =print_data
	bl printf

	mov x0, #0

	ldp  x29, x30, [sp], 16
	ret
