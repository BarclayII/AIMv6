/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <string.h>
#include <asm/addrspace.h>

/* The entry defined in except.S */
extern unsigned int except_generic;

void trap_init(void)
{
	/*
	 * MIPS defined exception handler entries in predefined addresses.
	 *
	 * The base address for all handlers is determined by Bootstrap
	 * Exception Vector (BEV) bit in CP0 status register.  If BEV=1,
	 * the base address would be 0xffffffffbfc00000, otherwise it would
	 * be 0xffffffff80000000.
	 *
	 * For TLB refill exception, the entry address is identical to base
	 * address.
	 *
	 * For 64-bit TLB refill exception, the entry address sometimes could
	 * be (base + 0x80), but here we copy the TLB refill handler to both
	 * entries nevertheless.
	 *
	 * For cache error exception, the entry is at (base + 0x100).  Usually
	 * this error should not happen, especially when cache coherency is
	 * maintained by hardware, such as Loongson 3A.
	 *
	 * All other exceptions, as well as interrupts, system calls, etc.,
	 * have their entries located at (base + 0x180).
	 */

	/*
	 * We temporarily use generic handler for TLB exceptions.
	 * Will be changed when implementing memory management.
	 */
	memcpy((void *)EXCEPT_TLB, &except_generic, 0x80);
	memcpy((void *)EXCEPT_TLBX, &except_generic, 0x80);
	memcpy((void *)EXCEPT_CACHEERR, &except_generic, 0x80);
	memcpy((void *)EXCEPT_GENERIC, &except_generic, 0x80);
}

/* Generic C exception handler */
void handle_exception(struct trapframe *tf)
{
}
