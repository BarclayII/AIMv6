/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <drivers/serial/uart16550.h>

extern void sata_print_cap(void);

void bootmain(void)
{
	uart_spin_puts("Entered bootmain()\r\n");
	uart_spin_printf("Jumping to C code at %08x\r\n", bootmain);

	sata_print_cap();

	for (;;)
		/* nothing */;
}
