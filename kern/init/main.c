/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

void main(void)
{
	/*
	 * Startup routine.
	 * Somehow combined the one in xv6 with that in Linux.
	 * @davidgao @leon001 please review
	 * 0.  Hardware probing etc.
	 * 1.  Trap
	 * 2.  Interrupt, including PIC, LAPIC, IOAPIC etc.
	 * 3.  Memory mapping, including page tables, allocators etc.
	 * 4.  Scheduler
	 * 5.  File system
	 * 6.  Disk storage
	 * 7.  SMP Startup
	 *     a)  Slave CPUs should skip step 8.
	 * 8.  Fork and load /sbin/init into process table
	 * 9.  Enable scheduler
	 */
}
