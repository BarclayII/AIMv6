/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <string.h>
#include <asm/mm/page.h>
#include <drivers/ata/ahci.h>

/*
 * ahci_init_port(port): software initialization of HBA port
 *
 * Return value:
 *     The content of SStatus register
 *
 * Reference:
 * SATA 1.3 specification
 */
unsigned int ahci_init_port(volatile struct ahci_hba_port *port)
{
	/* Physical address of a temporary FIS buffer */
	unsigned long fb = SATAPHY + CMDLIST_SIZE;
	struct fis_recvbuf *fb_buf = (struct fis_recvbuf *)p2kv(fb);

	/* Halt FIS and command processing */
	port->cmd &= ~(PORT_CMD_FRE | PORT_CMD_START);
	while (port->cmd & (PORT_CMD_CR | PORT_CMD_FR))
		/* nothing */;

	/* Reset port */
	port->scontrol = (port->scontrol & ~PORT_SCNTL_DET_MASK) |
	    PORT_SCNTL_DET_RESET;
	delay(1);
	port->scontrol = (port->scontrol & ~PORT_SCNTL_DET_MASK) |
	    PORT_SCNTL_DET_NONE;

	/* TODO: initialize FIS buffer */
	memset(fb_buf, 0, sizeof(struct fis_recvbuf));

	/* Enable FIS reception and spinup */
	port->cmd |= PORT_CMD_FRE | PORT_CMD_SUD;
	delay(1);
	return port->sstatus;
}
