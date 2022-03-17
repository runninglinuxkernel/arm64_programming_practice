
.global ldr_test
ldr_test:
	// 1. 测试ldr地址偏移模式
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

	ret
