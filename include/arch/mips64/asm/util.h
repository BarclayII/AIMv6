/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_UTIL_H
#define _ASM_UTIL_H

/* Align to 4 bytes */
#define FIX_INSTRUCTION_ADDR(x)		(((x) | 3) ^ 3)

#endif
