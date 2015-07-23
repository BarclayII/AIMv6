/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _PDEBUG_H
#define _PDEBUG_H

#include <config.h>

#ifdef DEBUG
#include <drivers/serial/uart.h>
#define pdebug(...)	uart_spin_printf(__VA_ARGS__)
#else	/* !DEBUG */
#define pdebug(...)
#endif	/* DEBUG */

#endif
