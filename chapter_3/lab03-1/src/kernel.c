#include "uart.h"

extern void ldr_test(void);

void my_ldr_str_test(void)
{

	ldr_test();
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
