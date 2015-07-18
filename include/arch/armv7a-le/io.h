/*
 * Copyright (C) 2015 David Gao <davidgao1001@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _IO_H
#define _IO_H

/*
 * Basic IO for ARMv7A Little Endian machines.
 * IO should always operate on unsigned data.
 */

unsigned char in8(unsigned long addr);
unsigned short in16(unsigned long addr);
unsigned long in32(unsigned int addr);

void out8(unsigned long addr, unsigned char data);
void out16(unsigned long addr, unsigned short data);
void out32(unsigned long addr, unsigned long data);

#endif

