/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _MACH_CONFIG_H
#define _MACH_CONFIG_H

/*
 * Loongson 3A specific configurations.
 *
 * The hardware register addresses are preset by the manufacturer.
 */

#define UART16550

#define UART0_BASE	0x1fe001e0

#define UART_BASE	UART0_BASE

#define RTC_MC146818

/* Loongson 3A increases COUNT register by one every 2 cycles */
#define COUNT_TO_FREQ(count)	((count) * 2)
#define FREQ_TO_COUNT(freq)	((freq) / 2)

#define NR_CPUS		4		/* 4 cores */

#endif
