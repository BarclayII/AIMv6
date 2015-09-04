/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <drivers/serial/uart.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>

ssize_t uart_spin_printf(const char *fmt, ...)
{
	int result;
	va_list ap;
	va_start(ap, fmt);
	result = uart_spin_vprintf(fmt, ap);
	va_end(ap);
	return result;
}

ssize_t uart_spin_vprintf(const char *fmt, va_list ap)
{
	int result;
	char printf_buf[BUFSIZ];
	result = vsnprintf(printf_buf, BUFSIZ, fmt, ap);
	uart_spin_puts(printf_buf);
	return result;
}
