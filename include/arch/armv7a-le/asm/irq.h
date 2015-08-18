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

/*
 * PLEASE READ ME! I AM SERIOUS!
 *
 * There are 2 methods to interrupt an ARMv7A core
 * from the outside world. IRQ and FIQ. Their control
 * contains 3 parts. See Cortex-A9 MPCore TRM and ARM
 * GIC Architecture specs before you go on.
 *
 * The core itself, through functions
 * {irq,fiq}_{en,dis}able()
 * They control CAN I BE INTERRUPTED BY ANY IRQ/FIQ
 *
 * The interrupt distributor, through functions
 * TODO
 * They control HOW TO DEAL WITH INTERRUPT REQUESTS
 * An ARM processor must only have 1 distributor.
 *
 * The CPU Interface, through functions
 * TODO
 * They control WHAT INTERRUPT REQUEST DO I WANT TO
 * SEE and performs actions like ACKNOWLEDGE and
 * END_OF_INTERRUPT
 */

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

