#define MY_LABEL 0x20

.global ldr_test
ldr_test:
	// lab1. 测试ldr地址偏移模式
	mov x1, 0x80000
	mov x3, 16

	/* 读取0x80000地址的值到x0寄存器*/
	ldr x0, [x1]

	/* 读取0x80008地址的值*/
	ldr x2, [x1, #8]

	/* 读取x1+x3 地址的值*/
	ldr x4, [x1, x3]

	/* 读取(x1+ x3<<3) 地址的值*/
	ldr x5, [x1, x3, lsl #3]

	// lab2：观察ldr前変基模式和后变基模式
	/* 前变基模式*/
	ldr x6, [x1, #8]!

	/* 后变基模式 */
	ldr x7, [x1], #8

	//lab3: 观察前变基和后变基的str指令

	/* 观测前变基的str，观察x2的值，地址0x400000的值 */
	mov x2, 0x400000
	ldr x6, =0x1234abce
	str x6, [x2, #8]! 

	/* 观测后变基的str，观察x2的值，地址0x500000的值 */
	mov x2, 0x500000
	str x6, [x2], #8

	//lab4: 观察ldr加载label
	/* 请观察 x6和x7的值*/
	ldr x6, MY_LABEL
	ldr x7, =MY_LABEL

	ret

/*
   lab5: 拷贝0x80000地址的内容到0x2000000地址，拷贝32字节
 */
.global my_memcpy_test
my_memcpy_test:
	mov x1, 0x80000
	mov x2, 0x200000
	add x3, x1, 32
1:
	ldr x4, [x1], #8
	str x4, [x2], #8
	cmp x1, x3
	b.cc 1b

	ret

/*lab7 : label access test*/
#define my_label 0x30

.globl string1
string1:
	.string "Boot at EL"

.align 3
.globl my_data
my_data:
	.word 0x44

.align 3
.global access_label_test
access_label_test:
	/*1. 加载一个很大的数*/
	ldr x1, =0xffff0000ffff0000

	/*2. 初始化寄存器的值，比特位操作*/
	ldr x2, =(1<<0) | (1<<2) | (1<<20) | (1<<40) | (1<<55)

	//这是错误的访问方式，mov指令只能访问16bit数据
	//mov x2, (1<<0) | (1<<2) | (1<<20) | (1<<40) | (1<<55)

	/*3. 访问一个宏定义 */
	ldr x0, =my_label  /*把宏的值加载到x0寄存器*/
	ldr x1, my_label  /* PC值 + 宏的值，然后加载这个地址的值到x1寄存器*/

	/*4. 访问一个字符串*/
	ldr x0, string1  /*加载string1的ASCII码的值到x0，加载8个字节的数据*/
	ldr x1, =string1 /*加载string1的地址到x1寄存器。
			   若读取地址的值，可以得到string1的ascii*/

	/*5. 访问一个data*/
	ldr x0, my_data  /*加载mydata的值到x0*/
	ldr x1, =my_data  /*加载mydata的地址到x1*/

	ret

/*
   测试adds, cmp以及adc指令的条件标志位
*/
.global add_inst_test
add_inst_test:
	mov x0, #0
	ldr x1, =0xffffffffffffffff
	mov x2, #3
	/*测试adds的进位功能，当有溢出发生时，C=1*/
	adds x0, x1, x1
	adc x3, xzr, xzr

	/*测试cmp的条件标志位，
	     当x1 > x2时，C=1
	     当x1 < x2是， C=0
	*/
	cmp x1, x2
	adc x4, xzr, xzr

	ret

/*
   当arg1 >= arg2 时， 返回 0
   当arg1 < arg2 时，返回0xffffffffffffffff
*/
.global compare_and_return
compare_and_return:
	cmp x0, x1
	sbc x0, xzr, xzr
	ret
