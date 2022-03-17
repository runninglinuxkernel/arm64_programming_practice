#include "uart.h"

extern void ldr_test(void);
extern void my_memcpy_test(void);

void my_ldr_str_test(void)
{

	ldr_test();
	my_memcpy_test();

	memset(0x200004, 0x55, 102);
}

void kernel_main(void)
{
	uart_init();
	uart_send_string("Welcome BenOS!\r\n");

	/* my test*/
	my_ldr_str_test();

	while (1) {
		uart_send(uart_recv());
	}
}
