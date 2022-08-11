SECTIONS
{
	/*
	 * 设置benos的加载入口地址为0x80000
	 *
	 * 这里“.”表示location counter，当前位置
	 */
	. = 0x80000,
	/*
	 * 这里是第一个段text.boot，起始地址就是0x80000
	 * 这个段存放了benos的第一条指令
	 */
	.text.boot : { *(.text.boot) }

	/*
	 * text代码段
	 */
	.text : { *(.text) }

	/*
	 * 只读数据段
	 */
	.rodata : { *(.rodata) }

	/*
	 * 数据段
	 */
	.data : { *(.data) }

	/*
	 * bss段
	 *
	 * ALIGN(8)表示8个字节对齐
	 * bss_begin的起始地址以8字节对齐
	 */
	. = ALIGN(0x8);
	bss_begin = .;
	.bss : { *(.bss*) } 
	bss_end = .;
	
	/*
	 * 分配一page的空间，用来存放页表
	 *
	 * 起始地址需要以page对齐
	 */
	. = ALIGN(4096);
	init_pg_dir = .;
	. += 4096;
}
