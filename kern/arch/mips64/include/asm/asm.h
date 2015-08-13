/*
 * asmmacro.h: Assembler macros to make things easier to read.
 *
 * Copyright (C) 1996 David S. Miller (dm@engr.sgi.com)
 * Copyright (C) 1998 Ralf Baechle
 *
 * Copyright (C) 2015 Gan Quan (coin2028@hotmail.com)
 */

#ifndef _ASM_ASM_H
#define _ASM_ASM_H

#include "regdef.h"
#include "cp0regdef.h"

/*
 * LEAF - declare leaf routine
 */
#define LEAF(symbol)                                    \
                .globl  symbol;                         \
                .align  2;                              \
                .type   symbol,@function;               \
                .ent    symbol,0;                       \
symbol:         .frame  sp,0,ra

/*
 * NESTED - declare nested routine entry point
 */
#define NESTED(symbol, framesize, rpc)                  \
                .globl  symbol;                         \
                .align  2;                              \
                .type   symbol,@function;               \
                .ent    symbol,0;                       \
symbol:         .frame  sp, framesize, rpc


/*
 * END - mark end of function
 */
#define END(function)                                   \
                .end    function;                       \
                .size   function,.-function

/*
 * OFFSET - specify address offset of the following code
 */
#define OFFSET(off)					\
		.org	off;

/*
 * ENTRY, EXIT - define entry and exit of a control block.
 * Use LEAF() or NESTED() instead if you want to declare an assembly function.
 */
#define ENTRY(symbol)					\
		.ent	symbol;

#define EXIT(symbol)					\
		.end	symbol;

#endif
