/*
 * Copyright (C) 2015 David Gao <davidgao1001@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <config.h>
#include <io.h>

#ifdef UART_ZYNQ7000

#include <drivers/serial/uart-zynq7000.h>

void uart_init(void)
{
}

void uart_enable(void)
{
	/* Enable TX and RX */
	out8()
}

void uart_disable(void)
{
}

unsigned char uart_spin_getbyte(void)
{
}

void uart_spin_putbyte(unsigned char byte)
{
}

#endif	/* UART_ZYNQ7000 */
