/*
 * Copyright (C) 2015 David Gao <davidgao1001@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_MACH_CONFIG_H
#define _ASM_MACH_CONFIG_H

#define UART_ZYNQ7000

#define UART0_BASE	0xe0000000
#define UART1_BASE	0xe0001000

/* ZYNQ USE UART1 AND NOT UART0 BY DEFAULT */
#define UART_BASE	UART1_BASE

#endif

