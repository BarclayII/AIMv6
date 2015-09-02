/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_IO_H
#define _ASM_IO_H

#include <sys/types.h>
#include <asm/addrspace.h>

#define iomap(addr)		(IO_BASE + (addr))

/*
 * in8() and out8() for MIPS receives physical address as parameter
 */
#define in8(addr)		\
	(*(volatile uchar *)iomap(addr))
#define out8(addr, data)	\
	((*(volatile uchar *)iomap(addr)) = (data))

#define in16(addr)		\
	(*(volatile ushort *)iomap(addr))
#define out16(addr, data)	\
	((*(volatile ushort *)iomap(addr)) = (data))

#define in32(addr)		\
	(*(volatile uint *)iomap(addr))
#define out32(addr, data)	\
	((*(volatile uint *)iomap(addr)) = (data))

#endif
