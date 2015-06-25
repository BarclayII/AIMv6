/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _MATH_H
#define _MATH_H

/*
 * Well, this math.h actually differs from C Standard, where it defines
 * division, modulo, bit operation macros as well.
 * Floating point operations are not necessary for operating system
 * design.
 */

/*
 * Include the architecture-specific math.h first
 */
#include <asm/math.h>

/*
 * [davidgao]
 * about integer division and modulo, as MIPS and X86 both have their hardware
 * solution, ARM should possibly get a coprocessor working.  Packaging that
 * into a function may be good, so we'd better use macros like div() or mod()
 * to keep C code alike on different platforms.
 *
 * TODO: add ARM division and modulo implementation in the following header:
 * arch/arm/include/asm/math.h
 */

#ifndef DIV
#define DIV(a, b)	((a) / (b))
#endif

#ifndef MOD
#define MOD(a, b)	((a) % (b))
#endif

#endif
