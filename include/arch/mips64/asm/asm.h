/*
 * asmmacro.h: Assembler macros to make things easier to read.
 *
 * Copyright (C) 1996 David S. Miller (dm@engr.sgi.com)
 * Copyright (C) 1998 Ralf Baechle
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 */

#ifndef _ASM_ASM_H
#define _ASM_ASM_H

#include <asm/regdef.h>
#include <asm/cp0regdef.h>

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
 * ls - Load Symbol pseudo-instruction
 *
 * GCC 5 and newer versions support direct loading with li pseudo-instruction
 * like:
 *
 * 	li	t0, __end__
 *
 * where __end__ is a symbol.
 *
 * However, versions around GCC 4 doesn't, although symbol address could be
 * obtained in a slightly more awkward manner:
 *
 * 	la	t0, __end__(zero)
 *
 * ls pseudo-instruction is thus defined here to increase readability.
 */
		.macro	ls	reg sym
		la	\reg, \sym($0)
		.endm

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
