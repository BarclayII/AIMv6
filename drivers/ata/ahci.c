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
#include <errno.h>
#include <pdebug.h>
#include <asm/mm/page.h>
#include <drivers/ata/ahci.h>
#include <drivers/serial/uart.h>

static unsigned long ahci_current_rambase = AHCIPHY;

static bool ahci_port_startable(volatile struct ahci_hba_port *port)
{
	unsigned int tfd_stat = ahci_port_tfd_status(port);
	unsigned int sstatus_det = port->sstatus & PORT_SSTAT_DET_MASK;
	unsigned int sstatus_ipm = port->sstatus & PORT_SSTAT_IPM_MASK;

	return (!(port->cmd & PORT_CMD_CR) &&
	    !(tfd_stat & FIS_STAT_BUSY) &&
	    !(tfd_stat & FIS_STAT_DRQ) &&
	    ((sstatus_det == PORT_SSTAT_DET_CONNECT) ||
	    (sstatus_ipm == PORT_SSTAT_IPM_PARTIAL) ||
	    (sstatus_ipm == PORT_SSTAT_IPM_SLUMBER) ||
	    (sstatus_ipm == PORT_SSTAT_IPM_DEVSLP)));
}

static inline void ahci_wait_port(volatile struct ahci_hba_port *port)
{
	unsigned int stat;

	do {
		stat = ahci_port_tfd_status(port);
	} while (stat & (FIS_STAT_BUSY | FIS_STAT_DRQ | FIS_STAT_ERR));
}

static void ahci_port_start(volatile struct ahci_hba_port *port)
{
	while (!ahci_port_startable(port))
		/* nothing */;
	port->cmd |= PORT_CMD_FRE;
	port->cmd |= PORT_CMD_START;
}

void ahci_identify_device(volatile struct ahci_hba_port *port)
{
	/* Take command slot 0 */
	unsigned int clb = port->clb;
	unsigned int csb = clb - AHCI_CLB_OFFSET;
	unsigned int db = clb - AHCI_CLB_OFFSET + AHCI_DATA_OFFSET;
	struct ahci_cmdhdr *ch = (struct ahci_cmdhdr *)p2kv(clb);
	struct ahci_cmdslot *cs = (struct ahci_cmdslot *)p2kv(csb);
	unsigned short *data = (unsigned short *)p2kv(db);
	unsigned int fb = port->fb;
	int i;

	/* Fill in PRD */
	memset(cs, 0, sizeof(*cs));
	cs->prd[0].dba = db;
	cs->prd[0].dbc = 256 * sizeof(unsigned short) - 1;	/* 256 words */
	/*
	 * Fill in command table.
	 * We need to send the IDENTIFY DEVICE command to HBA.
	 */
	cs->ct.cfis.type = FIS_H2D_REG;
	cs->ct.cfis.event = FIS_WRITE_CMD;
	cs->ct.cfis.cmd = ATA_IDENTIFY_DEVICE;
	cs->ct.cfis.lba.mode = 1;
	cs->ct.cfis.nsec_l = 1;

	/* Fill in command header */
	memset(ch, 0, sizeof(*ch));
	ch->cfl = sizeof(cs->ct.cfis) / sizeof(unsigned int);
	ch->clear_on_ok = 1;
	ch->prdtl = 1;
	ch->prefetch = 1;
	ch->ctba = kv2p(&cs->ct);
	ch->ctbau = 0;

	ahci_wait_port(port);
	memset(data, 0, sizeof(*data) * 256);

	/* Issue command! */
	port->cmd_issue = 1;

	/* Wait for command to complete */
	do {
		if (port->intr_status) {
			pdebug("\t\tInterrupt status %08x\r\n",
			    port->intr_status);
			pdebug("\t\tSError %08x\r\n", port->serror);
			delay(10000);
		}
	} while (port->cmd_issue & 1);
	if (port->intr_status) {
		pdebug("\t\tInterrupt status %08x\r\n",
		    port->intr_status);
		pdebug("\t\tSError %08x\r\n", port->serror);
		delay(10000);
	}

	/* Read data stored at the location indicated by PRD */
	uart_spin_printf("\t\tIdentification Data\r\n");
	for (i = 0; i < 256; ++i, ++data)
		uart_spin_printf("%04x%s", *data, (i + 1) % 8 ? " " : "\r\n");

	uart_spin_printf("%02x\r\n", *(unsigned char *)p2kv(fb));
}

/*
 * ahci_init_port(port):
 * Software initialization of HBA port
 *
 * Parameters:
 * @port	The HBA port structure
 * @serror	Returns SError register
 *
 * Return value:
 *     Zero if success, or -ENODEV if failure
 *
 * Reference:
 * SATA 1.3 specification
 */
int ahci_init_port(volatile struct ahci_hba_port *port, unsigned int *serror)
{
	/* Physical address of a temporary FIS buffer */
	unsigned int sstatus_det;
	unsigned long fb = ahci_current_rambase + AHCI_FB_OFFSET;
	unsigned long clb = ahci_current_rambase + AHCI_CLB_OFFSET;
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
	memset(fb_buf, 0, sizeof(*fb_buf));
	port->fbu = 0;
	port->fb = fb;

	/* Enable FIS reception and spinup */
	port->cmd |= PORT_CMD_FRE | PORT_CMD_SUD;

	/* Wait and see if device could be connected */
	delay(1000);
	sstatus_det = port->sstatus & PORT_SSTAT_DET_MASK;
	switch (sstatus_det) {
	case PORT_SSTAT_DET_NONE:
	case PORT_SSTAT_DET_OFFLINE:
		return -ENODEV;
		break;
	case PORT_SSTAT_DET_PRESENT:
	case PORT_SSTAT_DET_CONNECT:
		if (serror != NULL)
			*serror = port->serror;
		port->serror = 0;
		port->intr_status = 0;

		uart_spin_printf("Signature: %08x\r\n", port->sig);

		pdebug("Waiting...\r\n");
		ahci_wait_port(port);

		/* Specify command list buffer */
		port->clbu = 0;
		port->clb = clb;

		ahci_port_start(port);
		pdebug("Port started\r\n");

		ahci_identify_device(port);

		ahci_current_rambase += AHCI_PORT_RAM_SIZE;
		return 0;
		break;
	default:
		/* NOTREACHED */
		return -EINVAL;
		break;
	}
}

