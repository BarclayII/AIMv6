/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <config.h>

#ifdef UART16550

#include <sys/types.h>
#include <drivers/serial/uart16550.h>

void uart_init(void)
{
}

void uart_enable(void)
{
	register unsigned char mcr = UART_READ(UART_MODEM_CONTROL);
	UART_WRITE(UART_MODEM_CONTROL, mcr | UART_MCR_RTSC | UART_MCR_DTRC);
}

void uart_disable(void)
{
	register unsigned char mcr = UART_READ(UART_MODEM_CONTROL);
	UART_WRITE(UART_MODEM_CONTROL, mcr & ~(UART_MCR_RTSC | UART_MCR_DTRC));
}

unsigned char uart_spin_getbyte(void)
{
	while (!(UART_READ(UART_LINE_STATUS) & UART_LSR_DATA_READY))
		/* nothing */;
	return UART_READ(UART_RCV_BUFFER);
}

void uart_spin_putbyte(unsigned char byte)
{
	while (!(UART_READ(UART_LINE_STATUS) & UART_LSR_THRE))
		/* nothing */;
	UART_WRITE(UART_TRANS_HOLD, byte);
}

#endif	/* UART16550 */
