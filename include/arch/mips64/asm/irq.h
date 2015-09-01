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

void mach_init_irq(void);

#endif
