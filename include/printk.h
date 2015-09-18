/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _PRINTK_H
#define _PRINTK_H

#include <stdarg.h>
#include <sys/types.h>
#include <drivers/serial/uart.h>

static inline ssize_t vprintk(const char *fmt, va_list ap)
{
	return uart_spin_vprintf(fmt, ap);
}

static inline ssize_t printk(const char *fmt, ...)
{
	ssize_t result;
	va_list ap;
	va_start(ap, fmt);
	result = vprintk(fmt, ap);
	va_end(ap);
	return result;
}

#endif
