/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_IO_H
#define _ASM_IO_H

/*
 * TODO:
 * Change these header locations after you organize them
 */
#include "sys/types.h"
#include "addrspace.h"

#define iomap(addr)		(IO_BASE + (addr))

#endif
