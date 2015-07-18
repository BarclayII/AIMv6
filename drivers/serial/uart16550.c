/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <drivers/serial/uart16550.h>

unsigned char uart16550_spin_getbyte(void)
{
	while (!(UART_READ(UART_LINE_STATUS) & UART_LSR_DATA_READY))
		/* nothing */;
	return UART_READ(UART_RCV_BUFFER);
}

void uart16550_spin_putbyte(unsigned char byte)
{
	while (!(UART_READ(UART_LINE_STATUS) & UART_LSR_THRE))
		/* nothing */;
	UART_WRITE(UART_TRANS_HOLD, byte);
}

void uart_spin_puts(const char *str)
{
	for (; *str != '\0'; ++str)
		uart16550_spin_putbyte((unsigned char)*str);
}

ssize_t uart_spin_printf(const char *fmt, ...)
{
	int result;
	va_list ap;
	char printf_buf[BUFSIZ];
	va_start(ap, fmt);
	result = vsnprintf(printf_buf, BUFSIZ, fmt, ap);
	uart_spin_puts(printf_buf);
	va_end(ap);
	return result;
}
