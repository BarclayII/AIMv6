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

#ifndef _ASM_IO_H
#define _ASM_IO_H

#include <sys/types.h>
#include <asm/addrspace.h>

#define iomap(addr)		(IO_BASE + (addr))

/*
 * inx() and outx() for MIPS receives physical address as parameter
 */
#define inb(addr)		\
	(*(volatile uchar *)iomap(addr))
#define outb(addr, data)	\
	((*(volatile uchar *)iomap(addr)) = (data))

#define inw(addr)		\
	(*(volatile ushort *)iomap(addr))
#define outw(addr, data)	\
	((*(volatile ushort *)iomap(addr)) = (data))

#define ind(addr)		\
	(*(volatile uint *)iomap(addr))
#define outd(addr, data)	\
	((*(volatile uint *)iomap(addr)) = (data))

#define in8(addr)		inb(addr)
#define in16(addr)		inw(addr)
#define in32(addr)		ind(addr)

#define out8(addr, data)	outb(addr, data)
#define out16(addr, data)	outw(addr, data)
#define out32(addr, data)	outd(addr, data)

#endif
