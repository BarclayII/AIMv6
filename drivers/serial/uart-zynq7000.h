/*
 * Copyright (C) 2015 David Gao <davidgao1001@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _DRIVERS_SERIAL_UART_ZYNQ7000_H
#define _DRIVERS_SERIAL_UART_ZYNQ7000_H

/*
 * ZYNQ7000 UART driver
 */

/*
 * Include config.h to obtain UART base address (through
 * asm/mach-$MACH/config.h)
 */
#include <config.h>

void uart_init(void);
void uart_enable(void);
void uart_disable(void);
unsigned char uart_spin_getbyte(void);
void uart_spin_putbyte(unsigned char byte);

#endif
