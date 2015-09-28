/*
 * Copyright (C) 2015 David Gao <davidgao1001@gmail.com>
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
 * Include machine-specific configurations.
 */
#ifdef MACH_ZYNQ7000
#include <asm/mach-zynq7000/config.h>
#endif /* MACH_ZYNQ7000 */

#endif /* _ASM_CONFIG_H */

