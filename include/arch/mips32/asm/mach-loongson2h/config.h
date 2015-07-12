/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_MACH_CONFIG_H
#define _ASM_MACH_CONFIG_H

/*
 * Loongson 2H-specific configurations.
 *
 * The hardware register addresses are preset by the manufacturer.
 */

#define UART0_BASE	0x1fe80000
#define UART1_BASE	0x1fe81000
#define UART2_BASE	0x1fe82000
#define UART3_BASE	0x1fe83000

#define UART_BASE	UART0_BASE

/* The SATA address is virtual in Loongson 2H User Manual. */
#define SATA_BASE	0x1fe30000

#endif
