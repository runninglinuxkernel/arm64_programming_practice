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

void kernel_main(void)
{
	uart_init();
	uart_send_string("Welcome BenOS!\r\n");

	/* my test*/
	my_ldr_str_test();
	my_data_process_inst();

	while (1) {
		uart_send(uart_recv());
	}
}
