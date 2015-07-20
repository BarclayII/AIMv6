/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _DRIVERS_ATA_AHCI_H
#define _DRIVERS_ATA_AHCI_H

#include <drivers/ata/ahci_fis.h>
#include <drivers/ata/ahci_hba.h>

unsigned int ahci_init_port(volatile struct ahci_hba_port *port);

#endif
