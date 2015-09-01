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
 * There are 2 methods to interrupt an ARMv7A core from the outside world.
 * IRQ and FIQ. Their control contains 3 parts. See Cortex-A9 MPCore TRM and
 * ARM GIC Architecture specs before you go on.
 *
 * The core itself, through functions {irq,fiq,interrupt}_{en,dis}able()
 * They control CAN I BE INTERRUPTED BY ANY IRQ/FIQ
 *
 * The interrupt distributor, through functions TODO
 * They control HOW TO DEAL WITH INTERRUPT REQUESTS
 * An ARM processor must only have 1 distributor.
 *
 * The CPU Interface, through functions TODO
 * They control WHAT INTERRUPT REQUEST DO I WANT TO SEE and performs actions
 * like ACKNOWLEDGE and END_OF_INTERRUPT
 */

#include <asm/io.h>
#include <drivers/misc/dtb-zynq7000.h>

/* 
 * You won't want to use ICC and ICD outside the kernel, in places like MBR or
 * preload.
 */

#define ICC_PHYSBASE	(MPCORE_PHYSBASE + 0x0100)
#define ICD_PHYSBASE	(MPCORE_PHYSBASE + 0x1000)

/* FIXME is it good using macros here? */
#define icc_base	(mpcore_base + 0x0100)
#define icd_base	(mpcore_base + 0x1000)

/* ICC register offset */
#define ICC_ICR_OFFSET	0x00
#define ICC_PMR_OFFSET	0x04
#define ICC_BPR_OFFSET	0x08
#define ICC_IAR_OFFSET	0x0C
#define ICC_EOIR_OFFSET	0x10
#define ICC_RPR_OFFSET	0x14
#define ICC_HPIR_OFFSET	0x18
#define ICC_ABPR_OFFSET	0x1C
#define ICC_IDR_OFFSET	0x20

/* ICD register offset */
#define ICD_DCR_OFFSET	0x000
#define ICD_ICTR_OFFSET	0x004
#define ICD_IIDR_OFFSET	0x008
#define ICD_ISR_OFFSET	0x080	/* 32 * u32 regs */
#define ICD_ISER_OFFSET	0x100	/* 32 * u32 regs */
#define ICD_ICER_OFFSET	0x180	/* 32 * u32 regs */
#define ICD_ISPR_OFFSET	0x200	/* 32 * u32 regs */
#define ICD_ICPR_OFFSET	0x280	/* 32 * u32 regs */
#define ICD_ABR_OFFSET	0x300	/* 32 * u32 regs */
#define ICD_IPR_OFFSET	0x400	/* 255 * u32 regs */
#define ICD_IPTR_OFFSET	0x800	/* 255 * u32 regs */
#define ICD_ICFR_OFFSET	0xC00	/* 64 * u32 regs */
#define ICD_SGIR_OFFSET	0xF00

static inline void icc_enable()
{
	out32(icc_base + ICC_ICR_OFFSET, 0x1);
}

static inline void icc_disable()
{
	out32(icc_base + ICC_ICR_OFFSET, 0x0);
}

static inline void icd_enable()
{
	out32(icd_base + ICD_IDR_OFFSET, 0x1);
}

static inline void icd_disable()
{
	out32(icd_base + ICD_IDR_OFFSET, 0x0);
}

static inline void irq_enable()
{
	asm (
		"mrs r0, cpsr\n\t"
		"bic r0, r0, #0x80\n\t"
		"msr cpsr, r0\n\t"
		::: "r0"
	);
}

static inline void irq_disable()
{
	asm (
		"mrs r0, cpsr\n\t"
		"orr r0, r0, #0x80\n\t"
		"msr cpsr, r0\n\t"
		::: "r0"
	);
}

static inline void fiq_enable()
{
	asm (
		"mrs r0, cpsr\n\t"
		"bic r0, r0, #0x40\n\t"
		"msr cpsr, r0\n\t"
		::: "r0"
	);
}

static inline void fiq_disable()
{
	asm (
		"mrs r0, cpsr\n\t"
		"orr r0, r0, #0x40\n\t"
		"msr cpsr, r0\n\t"
		::: "r0"
	);
}

static inline void interrupt_enable()
{
	asm (
		"mrs r0, cpsr\n\t"
		"bic r0, r0, #0xC0\n\t"
		"msr cpsr, r0\n\t"
		::: "r0"
	);
}

static inline void interrupt_disable()
{
	asm (
		"mrs r0, cpsr\n\t"
		"orr r0, r0, #0xC0\n\t"
		"msr cpsr, r0\n\t"
		::: "r0"
	);
}

#endif /* _ASM_IRQ_H */

