#include "uart.h"

extern void ldr_test(void);
extern void my_memcpy_test(void);
extern void access_label_test(void);

void my_ldr_str_test(void)
{

	ldr_test();
	my_memcpy_test();

	memset(0x200004, 0x55, 102);

	access_label_test();


}

extern unsigned long compare_and_return(unsigned long a, unsigned long b);

extern void add_inst_test(void);
extern void ands_test(void);
extern void bitfield_test(void);
extern void cmp_cmn_test(void);
extern unsigned long csel_test(unsigned long r, unsigned long b);
extern void bl_test(void);
extern void adrp_test(void);
extern unsigned long my_atomic_write(long data);
extern void atomic_set(unsigned long a, unsigned long b);
void my_data_process_inst(void)
{
	unsigned long ret;
	unsigned long val;
	unsigned long p1 = 0;

	add_inst_test();

	ret = compare_and_return(10, 9);
	val = compare_and_return(9, 10);

	ands_test();
	bitfield_test();
	cmp_cmn_test();
	val = csel_test(1, 3);

	bl_test();

	adrp_test();

	val = my_atomic_write(0x345);

	atomic_set(0x11, &p1);
}

extern unsigned long func_addr[];
extern unsigned long func_num_syms;
extern char func_string[];

static int print_func_name(unsigned long addr)
{
	int i;
	char *p, *string;

	for (i = 0; i < func_num_syms; i++) {
		if (addr == func_addr[i])
			goto found;
	}

	return 0;

found:
    p = &func_string;

    while (1) {
    	p++;

    	if (*p == '\0')
    		i--;

    	if (i == 0) {
    		p++;
    		string = p;
		printk("<0x%lx> %s\n", addr, string);
    		break;
    	}
    }

    return 0;
}

extern unsigned long macro_test_1(long a, long b);
extern unsigned long macro_test_2(long a, long b);

extern char _text_boot[], _etext_boot[];
extern char _text[], _etext[];
extern char _rodata[], _erodata[];
extern char _data[], _edata[];
extern char _bss[], _ebss[];

static void print_mem(void)
{
	printk("BenOS image layout:\n");
	printk("  .text.boot: 0x%08lx - 0x%08lx (%6ld B)\n",
			(unsigned long)_text_boot, (unsigned long)_etext_boot,
			(unsigned long)(_etext_boot - _text_boot));
	printk("       .text: 0x%08lx - 0x%08lx (%6ld B)\n",
			(unsigned long)_text, (unsigned long)_etext,
			(unsigned long)(_etext - _text));
	printk("     .rodata: 0x%08lx - 0x%08lx (%6ld B)\n",
			(unsigned long)_rodata, (unsigned long)_erodata,
			(unsigned long)(_erodata - _rodata));
	printk("       .data: 0x%08lx - 0x%08lx (%6ld B)\n",
			(unsigned long)_data, (unsigned long)_edata,
			(unsigned long)(_edata - _data));
	printk("        .bss: 0x%08lx - 0x%08lx (%6ld B)\n",
			(unsigned long)_bss, (unsigned long)_ebss,
			(unsigned long)(_ebss - _bss));
}

void kernel_main(void)
{
	unsigned long val = 0;

	uart_init();
	init_printk_done();
	uart_send_string("Welcome BenOS!\r\n");
	printk("printk init done\n");

	/* my test*/
	my_ldr_str_test();
	my_data_process_inst();

	/*汇编器lab1：查表*/
	print_func_name(0x800880);
	val = macro_test_1(5, 5);
	val = macro_test_2(5, 5);

	print_mem();

	while (1) {
		uart_send(uart_recv());
	}
}
