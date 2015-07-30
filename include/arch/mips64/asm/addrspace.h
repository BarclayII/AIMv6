/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_ADDRSPACE_H
#define _ASM_ADDRSPACE_H

/*
 * MIPS32 virtual address space definition:
 *
 * Name		Address range		Access Mode	Cached	Mapping
 * USEG		0x00000000-0x7fffffff	U, S, K		Yes	TLB
 * KSEG0	0x80000000-0x9fffffff	K		Yes	Low 29 bits
 * KSEG1	0xa0000000-0xbfffffff	K		No	Low 29 bits
 * SSEG		0xc0000000-0xdfffffff	S, K		Yes	TLB
 * KSEG3	0xe0000000-0xffffffff	K		Yes	TLB
 *
 * Therefore, the first 512MB of physical addresses can be directly accessed
 * in MIPS32 by adding the physical address by KSEG0_BASE if one wants to
 * enable cache, or KSEG1_BASE if the access should be uncached.
 *
 * Conventionally, physical address 256MB-512MB (0x10000000-0x1fffffff) are
 * reserved for devices, so CPU could only access the lowest 256MB RAM
 * directly.  Physical addresses corresponding to higher RAM may be different
 * between machines.
 */

#define USEG_BASE	0x00000000
#define USEG_END	0x7fffffff
#define KSEG0_BASE	0x80000000
#define KSEG0_END	0x9fffffff
#define KSEG1_BASE	0xa0000000
#define KSEG1_END	0xbfffffff
#define SSEG_BASE	0xc0000000
#define SSEG_END	0xdfffffff
#define KSEG3_BASE	0xe0000000
#define KSEG3_END	0xffffffff

#define USEG		USEG_BASE
#define KSEG0		KSEG0_BASE
#define KSEG1		KSEG1_BASE
#define SSEG		SSEG_BASE
#define KSSEG		SSEG
#define KSEG3		KSEG3_BASE

#define IO_BASE		KSEG1
#define KERNEL_BASE	KSEG0

/* Physical address above MAXLOWERPHY are not linearly mapped in MIPS32 */
#define MAXLOWERPHY	0x20000000
#define DEVPHY		0x10000000
#define DMAPHY		0x0b000000

/*
 * Physical address space layout:
 * +------------------+	0		<--
 * |  Kernel memory   |			 |
 * +------------------+	DMAPHY		 Lower memory
 * | Device RAM (DMA) |			 |
 * +------------------+	DEVPHY		<--
 * | Device Registers |
 * +------------------+ MAXLOWERPHY
 * |       ...        |
 * +------------------+ UPPERMEMPHY
 * |    User memory   |
 * +------------------+ UPPERMEMPHY + RAMSIZE
 */

#endif
