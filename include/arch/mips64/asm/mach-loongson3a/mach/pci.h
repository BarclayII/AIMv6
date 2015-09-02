/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _MACH_PCI_H
#define _MACH_PCI_H

#define PCI_PORT_BASE	0x18000000

#define pcimap(port)	(PCI_PORT_BASE + (port))

#endif
