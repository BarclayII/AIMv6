/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_SMP_H
#define _ASM_SMP_H

#ifndef __ASSEMBLER__

#include <asm/mipsregs.h>

#ifndef cpuid

inline unsigned int __cpuid(void)
{
	return read_c0_ebase() & EBASE_CPUNUM_MASK;
}

#define cpuid()		__cpuid()
#endif	/* !defined(cpuid) */

#endif	/* !__ASSEMBLER__ */

#endif
