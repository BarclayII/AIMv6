

#include <drivers/serial/uart16550.h>

void bootmain(void)
{
	uart_spin_puts("Entered bootmain()\r\n");
	uart_spin_printf("Jumping to C code at %08x\r\n", bootmain);
	for (;;)
		/* nothing */;
}
