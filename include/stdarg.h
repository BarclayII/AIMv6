/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _STDARG_H
#define _STDARG_H

typedef __builtin_va_list	va_list;

#define va_start(ap, last)	(__builtin_va_start(ap, last))
#define va_arg(ap, type)	(__builtin_va_arg(ap, type))
#define va_end(ap)		(__builtin_va_end(ap))

#endif
