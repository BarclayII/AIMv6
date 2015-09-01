/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_PCI_H
#define _ASM_PCI_H

#include <mach/pci.h>
#include <asm/io.h>

/*
 * inb, inw, ind are equivalents for inb, inw, ind instructions on x86
 * for accessing PCI ports.
 *
 * MIPS PCI mapping is really machine-specific.
 */
#define inb(port)	in8(pcimap(port))
#define outb(port, val)	out8(pcimap(port), val)

#define inw(port)	in16(pcimap(port))
#define outw(port, val)	out16(pcimap(port), val)

#define ind(port)	in32(pcimap(port))
#define outd(port, val)	out32(pcimap(port), val)

#endif
