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
#include <asm/io.h>

void puthex(u32 num)
{
    int i;
    const char table[] = "0123456789ABCDEF";
    char buf[11] = "00000000\r\n";
    for (i = 28; i >= 0; i -= 4){
        buf[(28 - i) >> 2] = table[(num >> i) & 0xF];
    }
    uart_spin_puts(buf);
}

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
	else {
		uart_spin_puts("Preload: SD Card initialization failed.\r\n");
		goto spin;
	}
	
	u8 buf[512];
	/* MMU is off */
	ret = sd_dma_spin_read((u32)&buf, 1, 0);
	if (ret == 0) uart_spin_puts("Preload: SD Card read OK.\r\n");
	else {
		uart_spin_puts("Preload: SD Card read failed.\r\n");
		goto spin;
	}
	int i;
	for (i = 0; i < 512; i += 1) puthex(buf[i]);
spin:
	while (1);
}

