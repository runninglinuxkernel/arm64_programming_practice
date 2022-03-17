TEXT_ROM = 0x90000;

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
	_text_boot = .;
	.text.boot : { *(.text.boot) }
	_etext_boot = .;

	/*
	 * text代码段
	 */
	_text = .;
	.text :
	{
		*(.text)
	}
	_etext = .;

	/*
	 * 只读数据段
	 */
	_rodata = .;
	.rodata :
	{
		*(.rodata)
	}
	_erodata = .;


	/*
	 * 数据段
	 */
	_data = .;
	.data : { *(.data) }
	_edata = .;


	/*
	 * bss段
	 *
	 * ALIGN(8)表示8个字节对齐
	 * bss_begin的起始地址以8字节对齐
	 */
	. = ALIGN(0x8);
	_bss = .;
	.bss : { *(.bss*) } 
	_ebss = .;
	
	/*
	 * 分配一page的空间，用来存放页表
	 *
	 * 起始地址需要以page对齐
	 */
	. = ALIGN(4096);
	init_pg_dir = .;
	. += 4096;
}
