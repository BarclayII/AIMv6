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

#include <asm/asm.h>
#include <asm/regdef.h>
#include <asm/cp0regdef.h>

/*
 * Implementation of cpuid() macro and cpuid assembly macro to get the
 * current core ID#.
 */
#if defined(_MIPS_ARCH_MIPS32) || defined(_MIPS_ARCH_MIPS64)	/* Rev. 1 */
/*
 * Since MIPS32/64 Revision 1 don't provide EBASE register, how to retrieve
 * the core ID# is machine-dependent.
 * Provide __cpuid() function in mach/smp.h
 */
#include <mach/smp.h>
#else	/* Rev. 2 or higher */
/*
 * Revision 2 or higher provides the EBASE register, where the core ID#
 * is encoded.
 */
#ifndef __ASSEMBLER__
/*
 * The header is included by a C header/source.
 */
inline unsigned int __cpuid(void)
{
	return read_c0_ebase() & EBASE_CPUNUM_MASK;
}

#define cpuid()		__cpuid()
#else	/* __ASSEMBLER__ */
/*
 * The header is included by an assembly header/source.
 */
	.macro	cpuid result temp
	MFC0	\result, CP0_EBASE
	andi	\result, EBASE_CPUNUM_MASK
	.endm
#endif	/* !__ASSEMBLER__ */

#endif	/* Rev. */

#endif
