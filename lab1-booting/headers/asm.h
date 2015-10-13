/*
 * asmmacro.h: Assembler macros to make things easier to read.
 *
 * Copyright (C) 1996 David S. Miller (dm@engr.sgi.com)
 * Copyright (C) 1998 Ralf Baechle
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 */

#ifndef _ASM_ASM_H
#define _ASM_ASM_H

/*
 * TODO:
 * Change these header locations after you organize them
 */
#include "regdef.h"
#include "cp0regdef.h"

/*
 * LEAF - declare leaf routine
 */
#define LEAF(symbol)					\
		.globl	symbol;				\
		.align	2;				\
		.type	symbol, @function;		\
		.ent	symbol,0;			\
symbol:		.frame	sp,0,ra

/*
 * NESTED - declare nested routine entry point
 */
#define NESTED(symbol, framesize, rpc)			\
		.globl	symbol;				\
		.align	2;				\
		.type	symbol, @function;		\
		.ent	symbol,0;			\
symbol:		.frame	sp, framesize, rpc


/*
 * END - mark end of function
 */
#define END(function)					\
		.end	function;			\
		.size	function,.-function

/*
 * GCC supported pseudo-instructions which are commonly used for convenience:
 *
 * Mnemonic	Parameters	Description
 * --------------------------------------------------------------------------
 * BLT		r1, r2, label	Branch to label if r1 < r2
 * BGT		r1, r2, label	Branch to label if r1 > r2
 * BLE		r1, r2, label	Branch to label if r1 <= r2
 * BGE		r1, r2, label	Branch to label if r1 >= r2
 * NOT		rd, rs		rd = ~rs
 * NEG		rd, rs		rd = -rs
 * LI		rd, imm		rd = imm
 * LA		rd, off(rs)	rd = off + rs
 * MOVE		rd, rs		rd = rs
 * SLE		rd, rs, rt	rd = (rs <= rt) ? 1 : 0
 * SGE		rd, rs, rt	rd = (rs >= rt) ? 1 : 0
 * SGT		rd, rs, rt	rd = (rs > rt) ? 1 : 0
 *
 * These instructions do not exist in MIPS instruction set, and would be
 * translated to equivalent instruction combinations.
 *
 * Moreover, arithmetic/logical operations could have only two operands
 * instead of the standard three in GCC.  In this case, the ALU receives
 * both operands, compute the result, and store it back to the destination
 * register:
 * ADD		rd, rs		rd += rs
 */

/*
 * DEFINE_<TYPE> - Define a local <type> for current assembly source only
 * DEFINE_GLOBAL_<TYPE> - Define a global <type> shared across assembly files
 */
#define DEFINE_STRING(sym, str)				\
		.type	sym, @object;			\
		.align	2;				\
sym:		.ascii	str;				\
		.size	sym,.-sym;

#define DEFINE_GLOBAL_STRING(sym, str)			\
		.globl	sym;				\
		.type	sym, @common;			\
		.align	2;				\
sym:		.ascii	str;				\
		.size	sym,.-sym;

#endif

#define DEFINE_WORD(sym, word)				\
		.type	sym, @object;			\
		.align	2;				\
sym:		.word	word;

#define DEFINE_GLOBAL_WORD(sym, word)			\
		.globl	sym;				\
		.type	sym, @common;			\
		.align	2;				\
sym:		.word	word

#define DEFINE_LONG(sym, long)				\
		.type	sym, @object;			\
		.align	2;				\
sym:		.long	long;

#define DEFINE_GLOBAL_LONG(sym, long)			\
		.globl	sym;				\
		.align	2;				\
		.type	sym, @common;			\
sym:		.long	long

/*
 * Pseudo-instruction sets for compatibility
 *
 * Instruction not listed here doesn't have compatibility issues and
 * can be used as is.
 */

#if defined(_MIPS_ARCH_MIPS32) || defined(_MIPS_ARCH_MIPS32R2)
/* Load constant, See MIPS Run 8.7.3 */
# define LA		la
# define LI		li
/* Addition and subtraction, See MIPS Run 8.7.4 */
# define ADD		add
# define ADDI		addi
# define ADDU		addu
# define ADDIU		addiu
# define SUB		sub
# define SUBU		subu
/* Register shifts & rotates */
# define ROL		rol
# define ROR		ror
# define SLL		sll
# define SRA		sra
# define SRL		srl
/* Multiplication and division, See MIPS Run 8.7.5 */
# define DIV		div
# define DIVU		divu
# define MUL		mul
# define MULT		mult
# define MULTU		multu
# define REM		rem
# define REMU		remu
/* Load and Store, See MIPS Run 8.7.7 */
# define LOAD		lw
# define STORE		sw
# define LOADL		lwl
# define LOADR		lwr
# define STOREL		swl
# define STORER		swr
# define ULOAD		ulw
# define USTORE		usw
/* Load-link and Store-conditional */
# define LL		ll
# define SC		sc
/*
 * CP0 Transfers, See MIPS Run 8.7.10.
 *
 * Be sure to check the width of CP0 registers on MIPS64 CPUs, as not all
 * CP0 registers are 64-bit there.
 *
 * For example, CP0_STATUS and CP0_CAUSE are always 32-bit, but
 * CP0_EPC is 64-bit on MIPS64.
 */
# define MFC0		mfc0
# define MTC0		mtc0
#elif defined(_MIPS_ARCH_MIPS64) || defined(_MIPS_ARCH_MIPS64R2)
/* Load constant, See MIPS Run 8.7.3 */
# define LA		dla
# define LI		dli
/* Addition and subtraction, See MIPS Run 8.7.4 */
# define ADD		dadd
# define ADDI		daddi
# define ADDU		daddu
# define ADDIU		daddiu
# define SUB		dsub
# define SUBU		dsubu
/* Register shifts & rotates */
# define ROL		drol
# define ROR		dror
# define SLL		dsll
# define SRA		dsra
# define SRL		dsrl
/* Multiplication and division, See MIPS Run 8.7.5 */
# define DIV		ddiv
# define DIVU		ddivu
# define MUL		dmul
# define MULT		dmult
# define MULTU		dmultu
# define REM		drem
# define REMU		dremu
/* Load and Store, See MIPS Run 8.7.7 */
# define LOAD		ld
# define STORE		sd
# define LOADL		ldl
# define LOADR		ldr
# define STOREL		sdl
# define STORER		sdr
# define ULOAD		uld
# define USTORE		usd
/* Load-link and Store-conditional */
# define LL		lld
# define SC		scd
/* CP0 Transfers, See MIPS Run 8.7.10 */
# define MFC0		dmfc0
# define MTC0		dmtc0
#endif
