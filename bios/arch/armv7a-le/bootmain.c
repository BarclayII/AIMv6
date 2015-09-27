/*
 * Copyright (C) 2015 David Gao <davidgao1001@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <drivers/serial/uart.h>
#include <drivers/sd/sd-zynq7000.h>
#include <sleep.h>

void preload_bootmain(void)
{
	int ret;
	sleep(1);
	uart_init();
	uart_enable();
	uart_spin_puts("Preload: Hello!\r\n");
	sd_init();
	uart_spin_puts("Preload: SD Controller initialized.\r\n");
	ret = sd_spin_init_mem_card();
	if (ret == 0) uart_spin_puts("Preload: SD Card initialized.\r\n");
	else uart_spin_puts("Preload: SD Card bad.\r\n");
	while (1);
}

