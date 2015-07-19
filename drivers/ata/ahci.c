/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <drivers/ata/ahci.h>
#include <drivers/serial/uart.h>

void ahci_init_port(volatile struct ahci_hba_port *port)
{
	uart_spin_printf("CLB: %08x:%08x\r\n", port->clbu, port->clb);
	uart_spin_printf("FB : %08x:%08x\r\n", port->fb, port->fbu);
}
