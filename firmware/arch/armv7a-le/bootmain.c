/*
 * Copyright (C) 2015 David Gao <davidgao1001@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <sleep.h>
#include <asm/io.h>
#include <drivers/serial/uart.h>
#include <drivers/sd/sd-zynq7000.h>
#include <drivers/misc/dtb-zynq7000.h>

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

void firmware_bootmain(void)
{
	int ret;
	volatile u8 *mbr = (void *)0x100000; /* THIS IS NOT A NULL! */
	void (*mbr_entry)() = (void *)mbr;

	/* Wait for UART fifo to flush */
	sleep(1);

	/* Initialize and enable UART */
	uart_init();
	uart_enable();
	uart_spin_puts("FW: Hello!\r\n");

	/* Initialize SDHCI interface */
	sd_init();
	uart_spin_puts("FW: SD Controller initialized.\r\n");

	/* Initialize SD card */
	ret = sd_spin_init_mem_card();
	if (ret == 0) uart_spin_puts("FW: SD Card initialized.\r\n");
	else if (ret == 1) uart_spin_puts("FW: SDHC Card initialized.\r\n");
	else {
		uart_spin_puts("FW: Card initialization failed.\r\n");
		goto spin;
	}

	/*
	 * We don't turn on SCU now. The kernel should do this.
	 * This CANNOT be done here. DDR in 0x0 to 0xFFFFF is only accessible
	 * to processor cores, not the DMA controller.
	 * See Xilinx UG585, Table 4-1 for details.
	 */

	/* Read MBR */
	ret = sd_dma_spin_read((u32)mbr, 1, 0);
	if (ret == 0) uart_spin_puts("FW: Card read OK.\r\n");
	else {
		uart_spin_puts("FW: Card read failed.\r\n");
		goto spin;
	}
	
	/* Check MBR */
	if (mbr[510] == 0x55 && mbr[511] == 0xAA) {
		uart_spin_puts("FW: MBR valid.\r\n");
		mbr_entry();
	} else uart_spin_puts("FW: MBR not valid.\r\n");
	
spin:
	while (1);
}

