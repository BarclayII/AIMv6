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
 * Loongson 3A specific configurations.
 *
 * The hardware register addresses are preset by the manufacturer.
 */

#define UART16550

#define UART0_BASE	0x1fe001e0

#define UART_BASE	UART0_BASE

#endif
