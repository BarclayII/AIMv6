/*
 * Copyright (C) 2015 David Gao <davidgao1001@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_IO_H
#define _ASM_IO_H

/*
 * Basic IO for ARMv7A Little Endian machines.
 * IO should always operate on unsigned data.
 */

static inline unsigned char in8(unsigned long addr)
{
	return *(volatile unsigned char *)addr;
}

static inline unsigned short in16(unsigned long addr)
{
	return *(volatile unsigned short *)addr;
}

static inline unsigned long in32(unsigned long addr)
{
	return *(volatile unsigned long *)addr;
}


static inline void out8(unsigned long addr, unsigned char data)
{
	*(volatile unsigned char *)addr = data;
}

static inline void out16(unsigned long addr, unsigned short data)
{
	*(volatile unsigned short *)addr = data;
}

static inline void out32(unsigned long addr, unsigned long data)
{
	*(volatile unsigned long *)addr = data;
}

#endif /* _ASM_IO_H */

