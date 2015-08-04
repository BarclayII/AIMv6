/*
 * Copyright (C) 2015 David Gao <davidgao1001@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_IRQ_H
#define _ASM_IRQ_H

static inline void irq_enable()
{
    asm volatile (
        "mrs r0, cpsr;"
        "bic r0, r0, #0x80;"
        "msr cpsr, r0;"
    );
}

static inline void irq_disable()
{
    asm volatile (
        "mrs r0, cpsr;"
        "orr r0, r0, #0x80;"
        "msr cpsr, r0;"
    );
}

static inline void fiq_enable()
{
    asm volatile (
        "mrs r0, cpsr;"
        "bic r0, r0, #0x40;"
        "msr cpsr, r0;"
    );
}

static inline void fiq_disable()
{
    asm volatile (
        "mrs r0, cpsr;"
        "orr r0, r0, #0x40;"
        "msr cpsr, r0;"
    );
}

#endif /* _ASM_IRQ_H */

