/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _ASM_CONFIG_H
#define _ASM_CONFIG_H

/*
 * Machine selection goes here.
 *
 * Usually machine should be selected by specifying parameters in make, which
 * adds macro definitions through CFLAGS, rather than directly modifying the
 * code here.
 */

/* #define MACH_LOONGSON2H */	/* Loongson 2H */

/*
 * Include machine-specific configurations.
 */
#include <mach/config.h>

/* Other architecture configuration macros goes below. */

#endif
