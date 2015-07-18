/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

/*
 * Here is the place for holding equivalents of x86 primitives.
 */

#ifndef _ASM_PRIM_H
#define _ASM_PRIM_H

#include <sys/types.h>
#include <asm/addrspace.h>

/*
 * inx() and outx() for MIPS receives physical address as parameter
 */
#define inb(addr)		\
	(*(volatile uchar *)(IO_BASE + (addr)))
#define outb(addr, data)	\
	((*(volatile uchar *)(IO_BASE + (addr))) = (data))

#define inw(addr)		\
	(*(volatile ushort *)(IO_BASE + (addr)))
#define outw(addr, data)	\
	((*(volatile ushort *)(IO_BASE + (addr))) = (data))

#define ind(addr)		\
	(*(volatile uint *)(IO_BASE + (addr)))
#define outd(addr, data)	\
	((*(volatile uint *)(IO_BASE + (addr))) = (data))

#endif
