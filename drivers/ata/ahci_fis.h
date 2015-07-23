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
 * This header contains Frame Info Structure (FIS) definition for AHCI
 * controller.
 *
 * References:
 * SATA 1.0a specification
 * http://www.ece.umd.edu/courses/enee759h.S2003/references/serialata10a.pdf
 * (Oddly, not available on Intel or sata-io.org)
 *
 * SATA 1.1 specification, available on Intel website
 *
 * SATA Storage Technology, Don Anderson, Mindshare, 2007
 *
 * ATAPI 7 Specifications, Volume 3, available at
 * https://ata.wiki.kernel.org/index.php/Developer_Resources
 */

#ifndef _DRIVERS_ATA_AHCI_FIS_H
#define _DRIVERS_ATA_AHCI_FIS_H

#include <sys/types.h>

/*
 * FIS types
 */
#define FIS_H2D_REG		0x27	/* Host to Device Register */
#define FIS_D2H_REG		0x34	/* Device to Host Register */
#define FIS_SET_DEVICE_BITS	0xa1	/* D2H */
#define FIS_PIO_SETUP		0x5f	/* D2H*/
#define FIS_DMA_ACTIVATE	0x39	/* D2H */
#define FIS_DMA_SETUP		0x41	/* Bidirectional */
#define FIS_DATA		0x46	/* Bidirectional */
#define FIS_BIST_ACTIVATE	0x58	/* Bidirectional */

/* CHS representation used by FIS */
struct fis_chs {
	/* DWORD 1 */
	uchar		sec;		/* Sector [7..0] */
	uchar		cyl_l;		/* Cylinder [7..0] */
	uchar		cyl_h;		/* Cylinder [23..16] */
	uchar		head:4;		/* Head */
	uchar		dev:1;		/* Device */
	uchar		pad0:1;
	uchar		mode:1;		/* Must be 0 if in CHS mode */
	uchar		pad1:1;
	/* DWORD 2 */
	uchar		sec_e;		/* Sector [15..8] */
	uchar		cyl_el;		/* Cylinder [15..8] */
	uchar		cyl_eh;		/* Cylinder [31..24] */
	union {
		uchar	feat_e;		/* Features register [15..8] */
		uchar	rsvd;		/* Not used */
	};
};

/* LBA representation used by FIS */
struct fis_lba {
	/* DWORD 1 */
	uchar		lba0;		/* LBA [7..0] */
	uchar		lba2;		/* LBA [23..16] */
	uchar		lba4;		/* LBA [39..32] */
	uchar		pad0:4;
	uchar		dev:1;		/* Device */
	uchar		pad1:1;
	uchar		mode:1;		/* Must be 1 if in LBA mode */
	uchar		pad2:1;
	/* DWORD 2 */
	uchar		lba1;		/* LBA [15..8] */
	uchar		lba3;		/* LBA [31..24] */
	uchar		lba5;		/* LBA [47..40] */
	union {
		uchar	feat_e;		/* Features register [15..8] */
		uchar	rsvd;		/* Not used */
	};
};

/* Host to Device - usually for writing */
struct fis_reg_h2d {
	/* DWORD 0 */
	uchar		type;		/* FIS_H2D_REG */
	uchar		_pad0:7;
	uchar		event:1;	/* Transfer due to ... */
#define FIS_WRITE_CMD	1
#define FIS_WRITE_CTRL	0
	uchar		cmd;		/* Command register */
	uchar		feat;		/* Features register [7..0] */
	/* DWORD 1-2 */
	union {
		struct fis_chs chs;	/* Cylinder-Head-Sector */
		struct fis_lba lba;	/* Logical Block Address */
	};
	/* DWORD 3 */
	uchar		nsec_l;		/* Sector count [7..0] */
	uchar		nsec_h;		/* Sector count [15..8] */
	uchar		pad1;
	uchar		ctrl;		/* Control register */
#define FIS_CTRL_HOB	0x80		/* High Order Byte */
#define FIS_CTRL_SRST	0x04		/* Soft Reset */
#define FIS_CTRL_nIEN	0x02		/* Disable interrupt */
	/* DWORD 4 */
	uint		pad2;
};

/* Device to Host - usually for reading */
struct fis_reg_d2h {
	/* DWORD 0 */
	uchar		type;		/* FIS_D2H_REG */
	uchar		pad0:6;
	uchar		intr:1;		/* Interrupt bit */
	uchar		event:1;	/* Transfer due to ... */
	uchar		status;		/* Status register */
#define FIS_STAT_BUSY	0x80		/* Device busy */
#define FIS_STAT_DRDY	0x40		/* Device ready */
#define FIS_STAT_DF	0x20		/* Device fault */
#define FIS_STAT_SE	0x20		/* Stream error*/
#define FIS_STAT_SPEC	0x10		/* Command dependent */
#define FIS_STAT_DRQ	0x08		/* Data request */
#define FIS_STAT_ERR	0x01		/* Error reported */
#define FIS_STAT_CHK	0x01		/* Check exception */
	uchar		error;		/* Error register */
#define FIS_ERROR_ABRT	0x04		/* Operation aborted */
	/* DWORD 1-2 */
	union {
		struct fis_chs chs;	/* Cylinder-Head-Sector */
		struct fis_lba lba;	/* Logical Block Address */
	};
	/* DWORD 3 */
	uchar		nsec_l;		/* Sector count [7..0] */
	uchar		nsec_h;		/* Sector count [15..8] */
	ushort		pad1;
	/* DWORD 4 */
	uint		pad2;
};

/*
 * Received when a Queued DMA R/W or a 1st party DMA R/W is executed.
 * Each field is identical to the fields in H2D/D2H FISes with the same name.
 */
struct fis_set_dev_bits {
	/* DWORD 0 */
	uchar		type;		/* FIS_SET_DEVICE_BITS */
	uchar		pad0:5;
	uchar		pad1:1;
	uchar		intr:1;		/* Interrupt bit */
	uchar		pad2:1;
	uchar		status;		/* Status register w/o BUSY and DRQ */
	uchar		error;		/* Error register */
	/* DWORD 1 */
	union {
		uint	pad3;
		uint	sactive;	/* SActive bitmap for NCQ */
	};
};

struct fis_pio_setup {
	/* DWORD 0 */
	uchar		type;		/* FIS_PIO_SETUP */
	uchar		pad0:5;
	uchar		writemem:1;	/* Host memory being written by dev */
	uchar		intr:1;		/* Interrupt bit */
	uchar		pad1:1;
	uchar		begin_status;	/* Begin status */
	uchar		error;
	/* DWORD 1-2 */
	union {
		struct fis_chs chs;
		struct fis_lba lba;
	};
	/* DWORD 3 */
	uchar		nsec_l;
	uchar		nsec_h;
	uchar		pad2;
	uchar		end_status;	/* End status */
	/* DWORD 4 */
	ushort		nbytes;		/* a.k.a. Transfer Count */
	ushort		pad3;
};

struct fis_dma_setup {
	/* DWORD 0 */
	uchar		type;		/* FIS_DMA_SETUP */
	uchar		pad0:5;
	uchar		dir:1;		/* Direction */
#define FIS_DIR_SND2RCP	1		/* Sender to Recipient */
#define FIS_DIR_RCP2SND	0		/* Recipient to Sender */
	uchar		intr:1;		/* Interrupt after transmission */
	uchar		auto_act:1;	/* Auto-activate if supported */
	ushort		pad1;
	/* DWORD 1 */
	union {
		uint	dmabuf_l;	/* DMA Buffer Identifier Low */
		struct {
			uchar	tag;	/* TAG */
			uint	pad2:24;
		};
	};
	/* DWORD 2-6 */
	uint		dmabuf_h;	/* DMA Buffer Identifier High */
	uint		pad3;
	uint		dmabuf_off;	/* DMA Buffer Offset */
	uint		transfer_count;	/* DMA Transfer Count */
	uint		pad4;
};

struct fis_dma_activate {
	/* DWORD 0 */
	uchar		type;		/* FIS_DMA_ACTIVATE */
	uint		pad0:24;
};

struct fis_dma_data {
	/* DWORD 0 */
	uchar		type;		/* FIS_DMA_DATA */
	uint		pad0:24;
	/* May follow up to 2048 DWORDs */
};

struct fis_recvbuf {
	struct fis_dma_setup 	dsfis;
	uint			pad0[1];
	struct fis_pio_setup	psfis;
	uint			pad1[3];
	struct fis_reg_d2h	rfis;
	uint			pad2[1];
	struct fis_set_dev_bits	sdbfis;
	unsigned char		ufis[64];
	unsigned char		reserved[96];
};
#define AHCI_FIS_RECVBUF_SIZE	sizeof(struct fis_recvbuf)

#endif
