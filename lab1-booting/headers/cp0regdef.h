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
#ifndef _ASM_CP0REGDEF_H
#define _ASM_CP0REGDEF_H

/*
 * NOTE:
 * Not all of these are implemented in MSIM, but they're all available on
 * Loongson.
 */
#define CP0_INDEX	$0
#define CP0_RANDOM	$1
#define CP0_ENTRYLO0	$2
#define CP0_ENTRYLO1	$3
#define CP0_CONTEXT	$4
#define CP0_PAGEMASK	$5
# define CP0_PAGEGRAIN	$5, 1
#define CP0_WIRED	$6
#define CP0_HWRENA	$7
#define CP0_BADVADDR	$8	/* Bad Virtual Address Register */
#define CP0_COUNT	$9	/* Counter */
#define CP0_ENTRYHI	$10
#define CP0_COMPARE	$11	/* Comparer: raises interrupt when = $9 */
#define CP0_STATUS	$12	/* Status Register */
# define CP0_INTCTL	$12, 1	/* Interrupt control */
# define CP0_SRSCTL	$12, 2
# define CP0_SRSMAP	$12, 3
#define CP0_CAUSE	$13	/* Cause Register */
#define CP0_EPC		$14	/* Exception Program Counter */
#define CP0_PRID	$15
# define CP0_EBASE	$15, 1	/* Exception base, and CPU ID for multicore */
#define CP0_CONFIG	$16
# define CP0_CONFIG1	$16, 1
# define CP0_CONFIG2	$16, 2
# define CP0_CONFIG3	$16, 3
#define CP0_LLADDR	$17
#define CP0_WATCHLO	$18
#define CP0_WATCHHI	$19
#define CP0_XCONTEXT	$20
#define CP0_FRAMEMASK	$21
#define CP0_DIAGNOSTIC	$22
#define CP0_PERFCTL	$25
# define CP0_PERFCNT	$25, $1
#define CP0_ECC		$26
#define CP0_CACHEERR	$27
# define CP0_CERRADDR	$27, $1
#define CP0_TAGLO	$28
# define CP0_DATALO	$28, $1
#define CP0_TAGHI	$29
# define CP0_DATAHI	$29, $1
#define CP0_ERROREPC	$30
#define CP0_DESAVE	$31

/*
 * Status register (CP0_STATUS) mode bits
 */
#define ST_CU3	0x80000000	/* Coprocessor 3 (MIPS IV User Mode) */
#define ST_CU2	0x40000000	/* Coprocessor 2 */
#define ST_CU1	0x20000000	/* Coprocessor 1 (FPU) */
#define ST_CU0	0x10000000	/* Coprocessor 0 (this one) */
#define ST_RP	0x08000000	/* Reduce power */
#define ST_FR	0x04000000	/* Float register mode switch (?) */
#define ST_RE	0x02000000	/* Reverse-endian */
#define ST_MX	0x01000000	/* Enable DSP or MDMX */
#define ST_PX	0x00800000	/* Enable 64-bit operations in user mode */
/* The exception handler would be at 0xbfc00000 if BEV=1, 0x80000000 
 * otherwise */
#define ST_BEV	0x00400000	/* Bootstrap Exception Vector, usually 0 */
#define ST_TS	0x00200000	/* TLB SHUTDOWN */
#define ST_SR	0x00100000	/* Soft Reset */
#define ST_NMI	0x00080000	/* Non-maskable Interrupt */
/* Interrupt Masks */
#define ST_IM	0x0000ff00	/* All interrupt masks */
#define ST_IMx(i)	(1 << ((i) + 8))
/* eXtended addressing bits for 64-bit addresses */
#define ST_KX	0x00000080	/* Kernel mode eXtended addressing */
#define ST_SX	0x00000040	/* Supervisor mode eXtended addressing */
#define ST_UX	0x00000020	/* User mode eXtended addressing */
/*
 * This mask is helpful since clearing these bits in exception handler
 * guarantees that:
 * 1. The processor runs in kernel mode.
 * 2. The processor is safe from interrupts.
 * 3. The processor rewrites EPC when a nested exception occur.
 * 4. The processor can safely return to previous handler after dealing
 *    with nested ones.
 */
#define ST_EXCM	0x0000001f	/* Status Register EXception Clear Mask */
/* Kernel/Supervisor/User mode switch */
#define ST_KSU	0x00000018	/* KSU switch */
# define KSU_USER	0x00000010	/* User mode */
# define KSU_SUPERVISOR	0x00000008	/* Supervisor mode */
# define KSU_KERNEL	0x00000000	/* Kernel mode */
#define ST_ERL	0x00000004	/* Error Level */
#define ST_EXL	0x00000002	/* Exception Level */
#define ST_IE	0x00000001	/* Global Interrupt Enable */

#define NR_INTS	8		/* Number of Interrupt Mask Bits */

/*
 * Cause register (CP0_CAUSE) bits, for handling exceptions
 */

/* Branch Delay would be set if an exception occur in the delay slot, while
 * EPC points to the branching instruction. */
#define CR_BD	0x80000000	/* Branch Delay */
#define CR_TI	0x40000000	/* Timer Interrupt */
#define CR_CE	0x30000000	/* Coprocessor Error */
#define CR_DC	0x08000000	/* Disable Counter */
#define CR_PCI	0x04000000	/* CP0 Performance Counter Overflow (?) */
#define CR_IV	0x00800000	/* Interrupt Vector */
#define CR_WP	0x00400000	/* Watchpoint */
#define CR_IP	0x0000ff00	/* Interrupt Pending */
#define CR_IPx(i)	(1 << ((i + 8)))
#define CR_EC	0x0000007c	/* Exception Code */
#define EXCCODE(x)	(((x) & CR_EC) >> 2)

/*
 * Exception codes
 */
#define EC_int		0
#define EC_tlbm		1
#define EC_tlbl		2
#define EC_tlbs		3
#define EC_adel		4
#define EC_ades		5
#define EC_ibe		6
#define EC_dbe		7
#define EC_sys		8
#define EC_bp		9
#define EC_ri		10
#define EC_cpu		11
#define EC_ov		12
#define EC_tr		13
#define EC_fpe		15
#define EC_is		16
#define EC_dib		19
#define EC_ddbs		20
#define EC_ddbl		21
#define EC_watch	23
#define EC_dbp		26
#define EC_dint		27
#define EC_dss		28
#define EC_cacheerr	30

/*
 * PageMask register
 */

#define PM_4K		0x00000000
#define PM_8K		0x00002000
#define PM_16K		0x00006000
#define PM_32K		0x0000e000
#define PM_64K		0x0001e000
#define PM_128K		0x0003e000
#define PM_256K		0x0007e000
#define PM_512K		0x000fe000
#define PM_1M		0x001fe000
#define PM_2M		0x003fe000
#define PM_4M		0x007fe000
#define PM_8M		0x00ffe000
#define PM_16M		0x01ffe000
#define PM_32M		0x03ffe000
#define PM_64M		0x07ffe000
#define PM_256M		0x1fffe000
#define PM_1G		0x7fffe000

/*
 * PageGrain register used by Loongson 3A
 */

#define PG_ELPA		0x20000000	/* Enable Large Page Address */

/*
 * Config register
 */
#define CONF_CM		0x80000000	/* Config1 register (FPU) */
#define CONF_BE		0x00008000	/* Big-endianness */
#define CONF_AT		0x00006000	/* MIPS Architecture */
# define CONF_EM	0x00004000	/* MIPS64 with 64-bit address space */
# define CONF_EB	0x00002000	/* MIPS64 with 32-bit address space */
#define CONF_AR		0x00001c00	/* MIPS release version */
# define CONF_R2	0x00000400	/* MIPSr2 */
#define CONF_MT		0x00000380	/* MMU type */
# define CONF_TLB	0x00000080	/* Standard TLB */
#define CONF_VI		0x00000008	/* Virtual instruction cache */
#define CONF_K0		0x00000007	/* KSEG0 cache consistency */
# define CONF_CACHEABLE	0x00000003	/* Cacheable */
# define CONF_UNCACHED	0x00000002	/* Uncached */

#define EBASE_CPUNUM_MASK	0x3ff

#endif
