
#include <drivers/serial/uart.h>

void main(void)
{
	uart_spin_printf("Hello from kernel!\r\n");

	for (;;)
		/* nothing */;
}
