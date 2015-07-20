/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_MM_MMAP_H
#define _ASM_MM_MMAP_H

#include <asm/addrspace.h>

static inline void *p2kv(unsigned long paddr)
{
	return (paddr < HIGHPHY) ? (void *)(addr + KSEG0) : NULL;
}

static inline unsigned long kv2p(const void *kvaddr)
{
	return (kvaddr <= KSEG0_END && kvaddr >= KSEG0_BEGIN) ?
	    (unsigned long)kvaddr - KSEG0 : 0;
}

#endif
