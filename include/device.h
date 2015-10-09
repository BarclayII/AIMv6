/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _DEVICE_H
#define _DEVICE_H

#include <sys/types.h>

enum device_type {
	DEV_KBD,	/* Keyboard */
	DEV_LP,		/* Printer (MSIM) */
	DEV_VGA,	/* VGA (QEMU) */
	DEV_HD,		/* Hard disk (MSIM, QEMU, Loongson, ZYNQ) */
	DEV_SERIAL,	/* Serial */
	DEV_MPHUB,	/* Multi-processor Hub */
};

/*
 * Usage of struct device:
 *
 * This is a header structure for all device structures.
 * For example, one can define a structure:
 * struct hd {
 *      struct device dev;
 *      uint32_t size;
 *      ...
 * }
 * Then a pointer of "struct device" could also point to "struct hd".
 */
struct device {
	/*
	 * See major(3) and minor(3) for definition of major device number
	 * and minor device number.
	 */
	uint16_t	major;
	uint16_t	minor;
	uint16_t	type;		/* really should be enum device_type */
	uint16_t	irq_no;		/* negative for no IRQ */
	uint32_t	function;
#define DEVFUNC_LINE_OUTPUT	0x1
#define DEVFUNC_STORAGE		0x2
#define DEVFUNC_KEYBOARD	0x4
	uint32_t	status;
#define DEVSTAT_AVAILABLE	0x1
	unsigned long	phys_addr;	/* Device physical address */
};

/*
 * TODO:
 * @davidgao @raysmond
 * How shall we dispatch interrupts?
 * i386 has IRQs,
 * MIPS cores have 8 interrupt lines,
 * I wonder how ARM handles interrupts, and how shall we bend everything
 * together.
 * I guess the workflow should involve some interrupt handler registeration.
 */

struct device_intr_handler {
	bool		(*check)(struct device *);
	void		(*ack)(struct device *);
};

/*
 * Note:
 * Later the content inside the device array would be dynamically allocated,
 * after we implement kmalloc().
 * Maybe we should implement devs as a list instead.
 */
extern struct device *devs[];		/* Indexed by major device # */
extern unsigned int ndevs;		/* Number of devices registered */

/* Register a device into device list "devs" */
void device_register(struct device *dev);

#endif
