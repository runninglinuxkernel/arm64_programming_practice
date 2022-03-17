SECTIONS
{
	. = 0x80000,
	.text.boot : { *(.text.boot) }
	.text : { *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }
	. = ALIGN(0x8);
	bss_begin = .;
	.bss : { *(.bss*) } 
	bss_end = .;
	
	. = ALIGN(4096);
	init_pg_dir = .;
	. += 4096;
}
