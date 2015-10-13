/*
 * Copyright (C) 1994, 1995, 1996, 1997, 2000, 2001 by Ralf Baechle
 * Copyright (C) 2000 Silicon Graphics, Inc.
 * Modified for further R[236]000 support by Paul M. Antoine, 1996.
 * Kevin D. Kissell, kevink@mips.com and Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 2000, 07 MIPS Technologies, Inc.
 * Copyright (C) 2003, 2004  Maciej W. Rozycki
 *
 * Copyright (C) 2015, Gan Quan <coin2028@hotmail.com>
 */

#ifndef _ASM_MIPSREGS_H
#define _ASM_MIPSREGS_H

/*
 * TODO:
 * Change these header locations after you organize them
 */
#include "cp0regdef.h"
#include "irq.h"

#define _ZERO	0
#define _AT	1
#define _V0	2
#define _V1	3
#define _A0	4
#define _A1	5
#define _A2	6
#define _A3	7
#define _T0	8
#define _T1	9
#define _T2	10
#define _T3	11
#define _T4	12
#define _T5	13
#define _T6	14
#define _T7	15
#define _S0	16
#define _S1	17
#define _S2	18
#define _S3	19
#define _S4	20
#define _S5	21
#define _S6	22
#define _S7	23
#define _T8	24
#define _T9	25
#define _JP	25
#define _K0	26
#define _K1	27
#define _GP	28
#define _SP	29
#define _FP	30
#define _S8	30
#define _RA	31

/*
 * Macros to access the system control coprocessor
 */

#define __read_32bit_c0_register(source, sel)				\
({ int __res;								\
	if (sel == 0)							\
		__asm__ __volatile__(					\
			"mfc0\t%0, " #source "\n\t"			\
			: "=r" (__res));				\
	else								\
		__asm__ __volatile__(					\
			"mfc0\t%0, " #source ", " #sel "\n\t"		\
			: "=r" (__res));				\
	__res;								\
})

#define __read_64bit_c0_register(source, sel)				\
({ unsigned long long __res;						\
	if (sizeof(unsigned long) == 4)					\
		__res = __read_64bit_c0_split(source, sel);		\
	else if (sel == 0)						\
		__asm__ __volatile__(					\
			"dmfc0\t%0, " #source "\n\t"			\
			: "=r" (__res));				\
	else								\
		__asm__ __volatile__(					\
			"dmfc0\t%0, " #source ", " #sel "\n\t"		\
			: "=r" (__res));				\
	__res;								\
})

#define __write_32bit_c0_register(register, sel, value)			\
do {									\
	if (sel == 0)							\
		__asm__ __volatile__(					\
			"mtc0\t%z0, " #register "\n\t"			\
			: : "Jr" ((unsigned int)(value)));		\
	else								\
		__asm__ __volatile__(					\
			"mtc0\t%z0, " #register ", " #sel "\n\t"	\
			: : "Jr" ((unsigned int)(value)));		\
} while (0)

#define __write_64bit_c0_register(register, sel, value)			\
do {									\
	if (sizeof(unsigned long) == 4)					\
		__write_64bit_c0_split(register, sel, value);		\
	else if (sel == 0)						\
		__asm__ __volatile__(					\
			"dmtc0\t%z0, " #register "\n\t"			\
			: : "Jr" (value));				\
	else								\
		__asm__ __volatile__(					\
			"dmtc0\t%z0, " #register ", " #sel "\n\t"	\
			: : "Jr" (value));				\
} while (0)

/*
 * These versions are only needed for systems with more than 38 bits of
 * physical address space running the 32-bit kernel.  That's none atm :-)
 */
#define __read_64bit_c0_split(source, sel)				\
({									\
	unsigned long long __val;					\
	unsigned long __flags;						\
									\
	local_irq_save(__flags);					\
	if (sel == 0)							\
		__asm__ __volatile__(					\
			"dmfc0\t%M0, " #source "\n\t"			\
			"dsll\t%L0, %M0, 32\n\t"			\
			"dsra\t%M0, %M0, 32\n\t"			\
			"dsra\t%L0, %L0, 32\n\t"			\
			: "=r" (__val));				\
	else								\
		__asm__ __volatile__(					\
			"dmfc0\t%M0, " #source ", " #sel "\n\t"		\
			"dsll\t%L0, %M0, 32\n\t"			\
			"dsra\t%M0, %M0, 32\n\t"			\
			"dsra\t%L0, %L0, 32\n\t"			\
			: "=r" (__val));				\
	local_irq_restore(__flags);					\
									\
	__val;								\
})

#define __write_64bit_c0_split(source, sel, val)			\
do {									\
	unsigned long __flags;						\
									\
	local_irq_save(__flags);					\
	if (sel == 0)							\
		__asm__ __volatile__(					\
			"dsll\t%L0, %L0, 32\n\t"			\
			"dsrl\t%L0, %L0, 32\n\t"			\
			"dsll\t%M0, %M0, 32\n\t"			\
			"or\t%L0, %L0, %M0\n\t"				\
			"dmtc0\t%L0, " #source "\n\t"			\
			: : "r" (val));					\
	else								\
		__asm__ __volatile__(					\
			"dsll\t%L0, %L0, 32\n\t"			\
			"dsrl\t%L0, %L0, 32\n\t"			\
			"dsll\t%M0, %M0, 32\n\t"			\
			"or\t%L0, %L0, %M0\n\t"				\
			"dmtc0\t%L0, " #source ", " #sel "\n\t"		\
			: : "r" (val));					\
	local_irq_restore(__flags);					\
} while (0)

#define __read_ulong_c0_register(reg, sel)				\
	((sizeof(unsigned long) == 4) ?					\
	(unsigned long) __read_32bit_c0_register(reg, sel) :		\
	(unsigned long) __read_64bit_c0_register(reg, sel))

#define __write_ulong_c0_register(reg, sel, val)			\
do {									\
	if (sizeof(unsigned long) == 4)					\
		__write_32bit_c0_register(reg, sel, val);		\
	else								\
		__write_64bit_c0_register(reg, sel, val);		\
} while (0)

#define read_c0_index()		__read_32bit_c0_register($0, 0)
#define write_c0_index(val)	__write_32bit_c0_register($0, 0, val)

#define read_c0_random()	__read_32bit_c0_register($1, 0)
/*#define write_c0_random(val)	__write_32bit_c0_register($1, 0, val)*/

#define read_c0_entrylo0()	__read_ulong_c0_register($2, 0)
#define write_c0_entrylo0(val)	__write_ulong_c0_register($2, 0, val)

#define read_c0_entrylo1()	__read_ulong_c0_register($3, 0)
#define write_c0_entrylo1(val)	__write_ulong_c0_register($3, 0, val)

#define read_c0_context()	__read_ulong_c0_register($4, 0)
#define write_c0_context(val)	__write_ulong_c0_register($4, 0, val)

#define read_c0_pagemask()	__read_32bit_c0_register($5, 0)
#define write_c0_pagemask(val)	__write_32bit_c0_register($5, 0, val)

#define read_c0_pagegrain()	__read_32bit_c0_register($5, 1)
#define write_c0_pagegrain(val)	__write_32bit_c0_register($5, 1, val)

#define read_c0_wired()		__read_32bit_c0_register($6, 0)
#define write_c0_wired(val)	__write_32bit_c0_register($6, 0, val)

#define read_c0_hwrena()	__read_32bit_c0_register($7, 0)
#define write_c0_hwrena(val)	__write_32bit_c0_register($7, 0, val)

#define read_c0_badvaddr()	__read_ulong_c0_register($8, 0)

#define read_c0_count()		__read_32bit_c0_register($9, 0)
#define write_c0_count(val)	__write_32bit_c0_register($9, 0, val)

#define read_c0_entryhi()	__read_ulong_c0_register($10, 0)
#define write_c0_entryhi(val)	__write_ulong_c0_register($10, 0, val)

#define read_c0_compare()	__read_32bit_c0_register($11, 0)
#define write_c0_compare(val)	__write_32bit_c0_register($11, 0, val)

#define read_c0_status()	__read_32bit_c0_register($12, 0)
/*
 * Loongson documentation states that it detects and resolves CP0 hazards,
 * along with load hazards, in hardware, removing necessity of manual NOP
 * corrections.
 */
#define write_c0_status(val)						\
	__write_32bit_c0_register($12, 0, val);				\

#define read_c0_intctl()	__read_32bit_c0_register($12, 1)
#define write_c0_intctl(val)	__write_32bit_c0_register($12, 1, val)

#define read_c0_srsctl()	__read_32bit_c0_register($12, 2)
#define write_c0_srsctl(val)	__write_32bit_c0_register($12, 2, val)

#define read_c0_cause()		__read_32bit_c0_register($13, 0)
#define write_c0_cause(val)	__write_32bit_c0_register($13, 0, val)

#define read_c0_epc()		__read_ulong_c0_register($14, 0)
#define write_c0_epc(val)	__write_ulong_c0_register($14, 0, val)

#define read_c0_prid()		__read_32bit_c0_register($15, 0)

#define read_c0_ebase()		__read_32bit_c0_register($15, 1)
#define write_c0_ebase(val)	__write_32bit_c0_register($15, 1, val)

#define read_c0_config()	__read_32bit_c0_register($16, 0)
#define read_c0_config1()	__read_32bit_c0_register($16, 1)
#define read_c0_config2()	__read_32bit_c0_register($16, 2)
#define read_c0_config3()	__read_32bit_c0_register($16, 3)

#define write_c0_config(val)	__write_32bit_c0_register($16, 0, val)
#define write_c0_config1(val)	__write_32bit_c0_register($16, 1, val)
#define write_c0_config2(val)	__write_32bit_c0_register($16, 2, val)
#define write_c0_config3(val)	__write_32bit_c0_register($16, 3, val)

#define read_c0_lladdr()	__read_ulong_c0_register($17, 0)

#define read_c0_watchlo()	__read_ulong_c0_register($18, 0)
#define write_c0_watchlo(val)	__write_ulong_c0_register($18, 0, val)
#define read_c0_watchhi()	__read_32bit_c0_register($19, 0)
#define write_c0_watchhi(val)	__write_32bit_c0_register($19, 0, val)

#define read_c0_xcontext()	__read_ulong_c0_register($20, 0)
#define write_c0_xcontext(val)	__write_ulong_c0_register($20, 0, val)

#define read_c0_framemask()	__read_32bit_c0_register($21, 0)
#define write_c0_framemask(val)	__write_32bit_c0_register($21, 0, val)

#define read_c0_diag()		__read_32bit_c0_register($22, 0)
#define write_c0_diag(val)	__write_32bit_c0_register($22, 0, val)

#define read_c0_perfctrl0()	__read_32bit_c0_register($25, 0)
#define write_c0_perfctrl0(val)	__write_32bit_c0_register($25, 0, val)
#define read_c0_perfcntr0()	__read_32bit_c0_register($25, 1)
#define write_c0_perfcntr0(val)	__write_32bit_c0_register($25, 1, val)
#define read_c0_perfctrl1()	__read_32bit_c0_register($25, 2)
#define write_c0_perfctrl1(val)	__write_32bit_c0_register($25, 2, val)
#define read_c0_perfcntr1()	__read_32bit_c0_register($25, 3)
#define write_c0_perfcntr1(val)	__write_32bit_c0_register($25, 3, val)

#define read_c0_ecc()		__read_32bit_c0_register($26, 0)
#define write_c0_ecc(val)	__write_32bit_c0_register($26, 0, val)

#define read_c0_cacheerr()	__read_32bit_c0_register($27, 0)

#define read_c0_cerraddr()	__read_ulong_c0_register($27, 1)
#define write_c0_cerraddr(val)	__write_ulong_c0_register($27, 1, val)

#define read_c0_taglo()		__read_32bit_c0_register($28, 0)
#define write_c0_taglo(val)	__write_32bit_c0_register($28, 0, val)

#define read_c0_taghi()		__read_32bit_c0_register($29, 0)
#define write_c0_taghi(val)	__write_32bit_c0_register($29, 0, val)

#define read_c0_datalo()	__read_32bit_c0_register($28, 1)
#define write_c0_datalo(val)	__write_32bit_c0_register($28, 1, val)

#define read_c0_datahi()	__read_32bit_c0_register($29, 1)
#define write_c0_datahi(val)	__write_32bit_c0_register($29, 1, val)

#define read_c0_errorepc()	__read_ulong_c0_register($30, 0)
#define write_c0_errorepc(val)	__write_ulong_c0_register($30, 0, val)

#define read_c0_desave()	__read_ulong_c0_register($31, 0)
#define write_c0_desave(val)	__write_ulong_c0_register($31, 0, val)

#define __BUILD_SET_C0(name)					\
static inline unsigned int					\
set_c0_##name(unsigned int set)					\
{								\
	unsigned int res, new;					\
								\
	res = read_c0_##name();					\
	new = res | set;					\
	write_c0_##name(new);					\
								\
	return res;						\
}								\
								\
static inline unsigned int					\
clear_c0_##name(unsigned int clear)				\
{								\
	unsigned int res, new;					\
								\
	res = read_c0_##name();					\
	new = res & ~clear;					\
	write_c0_##name(new);					\
								\
	return res;						\
}								\
								\
static inline unsigned int					\
change_c0_##name(unsigned int change, unsigned int val)		\
{								\
	unsigned int res, new;					\
								\
	res = read_c0_##name();					\
	new = res & ~change;					\
	new |= (val & change);					\
	write_c0_##name(new);					\
								\
	return res;						\
}

__BUILD_SET_C0(status);
__BUILD_SET_C0(cause);
__BUILD_SET_C0(config);

#if 0
/*
 * These are for tools like ctags and cscope to conveniently locate the symbols.
 * See above for definition.
 */
#define set_c0_status
#define set_c0_cause
#define set_c0_config
#define clear_c0_status
#define clear_c0_cause
#define clear_c0_config
#define change_c0_status
#define change_c0_cause
#define change_c0_config
#endif


#endif
