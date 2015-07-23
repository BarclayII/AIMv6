/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

/*
 * This header contains SATA command structures and constants.
 *
 * References:
 * SATA 1.3 specification, available on Intel's website.
 */

#ifndef _DRIVERS_ATA_AHCI_CMD_H
#define _DRIVERS_ATA_AHCI_CMD_H

#include <sys/types.h>
#include <drivers/ata/ahci_fis.h>
#include <drivers/ata/ata_cmd.h>

/* PRD - Physical Region Descriptor */
struct ahci_prd {
	uint		dba;
	uint		dbau;
	uint		reserved;
	uint		dbc:22;		/* Data bytes minus 1 */
	uint		reserved2:9;
	uint		intr:1;
};

/*
 * Command Table
 *
 * The command table header is followed by a list of @prdtl PRDs.
 */
struct ahci_cmdtbl {
	union {
		struct fis_reg_h2d cfis;
		uchar	cfis_buf[64];
	};
	union {
		uchar	acmd_buf[16];
	};
	uchar		reserved[48];
};

struct ahci_cmdhdr {
	/* DWORD 0 */
	uchar		cfl:5;
	uchar		atapi:1;
	uchar		write:1;
	uchar		prefetch:1;
	uchar		reset:1;
	uchar		bist:1;
	uchar		clear_on_ok:1;
	uchar		rsvd:1;
	uchar		pmp:4;
	ushort		prdtl;		/* Should not exceed 8 */
	/* DWORD 1 */
	uint		prdbc;
	/* DWORD 2-3 */
	uint		ctba;
	uint		ctbau;
	/* DWORD 4-7 */
	uint		reserved[4];
};

struct ahci_cmdslot {
	struct ahci_cmdtbl	ct;
	struct ahci_prd 	prd[8];
};

#define AHCI_DATA_SIZE		0x200		/* 1 sector */
#define AHCI_CMDDESCLIST_SIZE	0x2000		/* See ahci.h */
#define AHCI_CMDDATALIST_SIZE	0x4000		/* 32 x Data FIS size */
#define AHCI_CMDLIST_SIZE	0x400		/* 32 x sizeof(ahci_cmdhdr) */

#endif
