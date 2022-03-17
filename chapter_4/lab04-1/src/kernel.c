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

extern void add_inst_test(void);
void my_data_process_inst(void)
{
	add_inst_test();
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
