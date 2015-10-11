/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <config.h>
#include <printk.h>
#include <asm/setup.h>
#include <asm/mipsregs.h>
#include <drivers/clock/rtc.h>

/* CPU pipeline frequency */
unsigned long cpu_freq;

#define NR_FREQ_SAMPLES	4

void probe_count_freq(void)
{
	unsigned int cnt, cnt_new, i, sec;
	unsigned long count_inc;
	cnt = read_c0_count();
	count_inc = 0;

	for (i = 0; i < NR_FREQ_SAMPLES + 1; ++i) {
		sec = rtc_getsecond();
		cnt = read_c0_count();
		do {
			cnt_new = read_c0_count();
		} while (rtc_getsecond() == sec);
		if (i != 0)
			count_inc += cnt_new - cnt;
	}

	count_inc /= NR_FREQ_SAMPLES;

	/* 
	 * The rate of increasing COUNT register on MIPS really depends on
	 * implementation.
	 */
	cpu_freq = COUNT_TO_FREQ(count_inc);
}

void clock_init(void)
{
	printk("Initializing Real Time Clock (RTC)...\r\n");
	rtc_init();
	printk(
	    "Current time: %04d-%02d-%02d %02d:%02d:%02d\r\n",
	    rtc_getyear(),
	    rtc_getmonth(),
	    rtc_getday(),
	    rtc_gethour(),
	    rtc_getminute(),
	    rtc_getsecond()
	);

	printk("Determining CPU frequency...\r\n");
	probe_count_freq();
	printk("CPU frequency: %u\r\n", cpu_freq);
}

void usleep(unsigned int usec)
{
	unsigned int cnt_incs = FREQ_TO_COUNT(cpu_freq / 1000000 * usec);
	unsigned int count = read_c0_count();
	for (; read_c0_count() - count < cnt_incs; )
		/* nothing */;
}

void setup_arch(void)
{
	int i, j;
	uint32_t bar;
	int port, slave;
	uint8_t stat;

	/* Initialize hardware clocks */
	clock_init();

	uint32_t addr;
	uint64_t addr64;

	addr = 0x1a000000 | (0x11 << 11) | (0x24);
	uint32_t bar5 = in32(addr);
	printk("SATA BAR5 = %08x\r\n", bar5);
	printk("SATA CAP = %08x\r\n", in32(bar5));
	printk("SATA GHC = %08x\r\n", in32(bar5 + 4));

	for (i = 0; i < 4; ++i) {
		addr = 0x1a000000 | (0x11 << 11) | (0x10 + 4 * i);
		bar = in32(addr) | 0x18000000;
		bar &= ~0x7;
		for (j = 0; j <= 1; ++j) {
			out8(bar + 0x6, 0xe0 + (j << 4));
			usleep(1000);
			stat = in8(bar + 0x7);
			printk("IDE DEV %d:%d STAT %02x\r\n", i, j, stat);
			if ((stat & 0x40) && !(stat & 0x21)) {
				port = i;
				slave = j;
			}
		}
	}

	printk("Setting up HT interrupts\r\n");

#define IO_REGS_BASE	0x3ff00000
	for (addr = 0x3ff01418; addr <= 0x3ff0141f; ++addr)
		out8(addr, 0x21);

	/*
	for (addr64 = 0xefdfb0000a0; addr64 <= 0xefdfb0000bc; addr64 += 4)
		out32(addr, 0xffffffff);
	*/
	out32(0xefdfb0000a0, 0xffffffff);
	uint32_t inten = in32(0x3ff01424);
	out32(0x3ff01428, inten | (0xffff << 16) | (1 << 10));

	printk("Setting up i8259A\r\n");
	/* i8259A, see picinit() in xv6 */
	out8(0xefdfc000021, 0xff);		/* PIC_MASTER_IMR */
	out8(0xefdfc0000a1, 0xff);		/* PIC_SLAVE_IMR */
	out8(0xefdfc000020, 0x11);		/* PIC_MASTER_CMD */
	out8(0xefdfc000021, 32);		/* T_IRQ0 */
	out8(0xefdfc000021, 1 << 2);		/* PIC_CASCADE_IR */
	out8(0xefdfc000021, 0x1);		/* Normal EOI as Linux/MIPS */
	out8(0xefdfc0000a0, 0x11);		/* PIC_SLAVE_CMD */
	out8(0xefdfc0000a1, 40);		/* T_IRQ0 + 8 */
	out8(0xefdfc0000a1, 2);
	out8(0xefdfc0000a1, 0x1);		/* ditto */
	/* not doing these as in Linux/MIPS */
	/*
	out8(0xefdfc000020, 0x68);
	out8(0xefdfc000020, 0x0a);
	out8(0xefdfc0000a0, 0x68);
	out8(0xefdfc0000a0, 0x0a);
	*/
	out8(0xefdfc000021, 0xff);
	out8(0xefdfc0000a1, 0xff);

	/* Identify device */
	uint32_t cause = read_c0_cause();
	printk("CAUSE BEFORE: %08x\r\n", cause);
	addr = 0x1a000000 | (0x11 << 11) | (0x10 + 4 * port);
	bar = in32(addr) | 0x18000000;
	bar &= ~0x7;
	addr = 0x1a000000 | (0x11 << 11) | (0x10 + 4 * (port + 1));
	uint32_t bar_ctrl = in32(addr) | 0x18000000;
	bar_ctrl &= ~0x7;
	printk("Identifying device...\r\n");
	out8(bar + 0x6, 0xe0 + (slave << 4));
	out8(bar_ctrl + 0x2, 0x0);	/* Interrupt */
	out8(bar + 0x7, 0xec);
	while (cause == read_c0_cause())
		/* nothing */;
	printk("CAUSE AFTER: %08x\r\n", read_c0_cause());
	if ((stat = in8(bar + 0x7)) & 0x01) {
		printk("ERROR?\r\n");
		return;
	}
	uint8_t buf, buf2;
	int cnt = 1;
	while (in8(bar + 0x7) & 0x08) {
		buf = in8(bar);
		buf2 = in8(bar);
		printk("%02x %02x%s", buf, buf2, cnt % 8 == 0 ? "\r\n" : " ");
		++cnt;
		usleep(1);
		if (cnt == 1000) {
			printk("Exceeds\r\n");
			break;
		}
	}
	printk("Finished\r\n");
}
