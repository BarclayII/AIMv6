/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_IRQ_H
#define _ASM_IRQ_H

#if defined(_MIPS_ARCH_MIPS32) || defined(_MIPS_ARCH_MIPS64)
#define local_irq_enable() \
	asm volatile ( \
		".set	push;" \
		".set	noat;" \
		"mfc0	$1, $12;" \
		"ori	$1, 1;" \
		"mtc0	$1, $12;" \
		".set	pop;" \
		: /* no output */ \
		: /* no input */ \
		: "$1", "memory" \
	)

#define local_irq_disable() \
	asm volatile ( \
		".set	push;" \
		".set	noat;" \
		"mfc0	$1, $12;" \
		"ori	$1, 1;" \
		"xori	$1, 1;" \
		"mtc0	$1, $12;" \
		".set	pop;" \
		: /* no output */ \
		: /* no input */ \
		: "$1", "memory" \
	)

#define local_irq_save(flags) \
	asm volatile ( \
		".set	push;" \
		".set	noat;" \
		"mfc0	%0, $12;" \
		"ori	$1, %0, 1;" \
		"xori	$1, $1, 1;" \
		"andi	%0, 1;" \
		"mtc0	$1, $12;" \
		".set	pop;" \
		: "=r"(flags) \
		: /* no input */ \
		: "$1", "memory" \
	)

#define local_irq_save(flags) \
	asm volatile ( \
		"	.set	push;" \
		"	.set	reorder;" \
		"	.set	noat;" \
		"	beqz	%0, 1f;" \
		"	mfc0	$1, $12;" \
		"	ori	$1, $1, 1;" \
		"	mtc0	$1, $12;" \
		"1f:	.set	pop;" \
		: /* no output */ \
		: "r"(flags) \
		: "$1", "memory" \
	)
#else
#define local_irq_enable()	asm volatile ("ei")

#define local_irq_disable()	asm volatile ("di")

#define local_irq_save(flags) \
	asm volatile ( \
		"di	%0;" \
		"andi	%0, 1;" \
		: "=r"(flags) \
		: /* no input */ \
		: "memory" \
	)

#define local_irq_restore(flags) \
	asm volatile ( \
		"	beqz	%0, 1f;" \
		"	di;" \
		"	ei;" \
		"1:" \
		: /* no output */ \
		: "r"(flags) \
		: "memory" \
	)
#endif

#endif
