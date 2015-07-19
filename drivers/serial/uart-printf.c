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
	char printf_buf[BUFSIZ];
	va_start(ap, fmt);
	result = vsnprintf(printf_buf, BUFSIZ, fmt, ap);
	uart_spin_puts(printf_buf);
	va_end(ap);
	return result;
}

