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
#include <drivers/ata/ahci_cmd.h>

int ahci_init_port(volatile struct ahci_hba_port *port, unsigned int *serror);

/*
 * See asm/addrspace.h for physical address space layout
 */

/*
 * Host memory layout for AHCI:
 *
 * Single port:
 * +-----------------------+ 0
 * |Command  1 Table Header|
 * +-----------------------+ 0x80
 * |Command  1   PRD  x  8 |
 * +-----------------------+ 0x100
 * |Command  2 Table Header|
 * +-----------------------+ 0x180
 * |Command  2   PRD  x  8 |
 * +-----------------------+ 0x200
 * |... (32 command tables)|
 * +-----------------------+ 0x2000 <--- DATA
 * |   Command  1   Data   |
 * +-----------------------+ 0x2200
 * |   Command  2   Data   |
 * +-----------------------+ 0x2400
 * | ... (32 command data) |
 * +=======================+ 0x6000 <--- CLB
 * |Command     Header    1|
 * +-----------------------+ 0x6020
 * |Command     Header    2|
 * +-----------------------+ 0x6040
 * |...(32 command headers)|
 * +=======================+ 0x6400 <--- FB
 * |FIS    Receive   Buffer|
 * +=======================+ 0x6500
 * |        Reserved       |
 * +-----------------------+ 0x8000
 */
#define AHCI_OFFSET		0x100000
#define AHCIPHY			(DMAPHY + AHCI_OFFSET)
#define AHCI_PORT_RAM_SIZE	0x8000
#define AHCI_DATA_OFFSET	0x2000
#define AHCI_CLB_OFFSET		0x6000
#define AHCI_FB_OFFSET		0x6400

extern unsigned int hba_ports_available;

#endif
