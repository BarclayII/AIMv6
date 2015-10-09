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
#include <stddef.h>
#include <printk.h>
#include <debug.h>
#include <smp.h>
/* 
 * TODO: add setup_arch() for hardware probing/detection/anything funny for
 * each architecture.  The function should be declared in asm/setup.h.
 */
#include <asm/setup.h>

union cpu_info_stack cpu_info_stack[NR_CPUS];

int main(void)
{
	/*
	 * 0.  Hardware probing etc.
	 * 1.  Trap
	 * 2.  SMP per-CPU setup
	 * 3.  Interrupt, including PIC, LAPIC, IOAPIC etc.
	 * 4.  Disk storage
	 * 5.  Memory mapping, including page tables, allocators etc.
	 *     a)  Swapping
	 * 6.  Scheduler
	 * 7.  File system
	 * 8.  SMP Startup
	 *     a)  Slave CPUs should skip to step 11
	 * 9.  Fork and load /sbin/init into process table
	 * 10. Enable swapping & scheduler
	 * 11. Enter idle stage
	 */
	printk("Hello from kernel!\r\n");

	/* 0. */
	setup_arch();
}

