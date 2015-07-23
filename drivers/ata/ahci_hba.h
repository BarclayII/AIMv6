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
 * This header contains SATA HBA register structures and constants.
 *
 * References:
 * SATA 1.1 specification, available on Intel's website.
 * SATA 1.3 specification, ditto
 */

#ifndef _DRIVERS_ATA_AHCI_HBA_H
#define _DRIVERS_ATA_AHCI_HBA_H

#include <asm/io.h>
#include <sys/types.h>
#include <stddef.h>
#include <drivers/ata/ahci_fis.h>

/*
 * Abbreviations:
 * CCC		- Command Completion Coalescing (not used)
 * EM		- Enclosure Management (not used)
 */

/*
 * HBA Per-port register structure
 */

struct ahci_hba_port {
	union {
		struct {
			uint	clb;		/* Command List Base Address */
			uint	clbu;		/* ... Upper */
			uint	fb;		/* FIS Base Address Lower */
			uint	fbu;		/* ... Upper */
			uint	intr_status;	/* Interrupt Status */
			uint	intr_enable;	/* Interrupt Enable */
			uint	cmd;		/* Command and Status */
			uint	reserved;
			uint	tfd;		/* Task file data */
			uint	sig;		/* Signature */
			uint	sstatus;	/* SStatus */
			uint	scontrol;	/* SControl */
			uint	serror;		/* SError */
			uint	sactive;	/* SActive */
			uint	cmd_issue;	/* Command issue */
			uint	snotification;	/* SNotification */
			uint	fbs;		/* FIS-based switch control */
			uint	dev_sleep;	/* Device sleep */
		};
		unsigned char	data[0x80];
	};
};

/* Interrupt Status and Interrupt Enable bits correspond to each other */
#define PORT_INTR_CPD	0x80000000	/* Cold Port Detect */
#define PORT_INTR_TFE	0x40000000	/* Task File Error */
#define PORT_INTR_HBF	0x20000000	/* Host Bus Fatal Error */
#define PORT_INTR_HBD	0x10000000	/* Host Bus Data Error */
#define PORT_INTR_IF	0x08000000	/* Interface Fatal Error */
#define PORT_INTR_INF	0x04000000	/* Interface Non-fatal Error */
#define PORT_INTR_OF	0x01000000	/* Overflow */
#define PORT_INTR_IPM	0x00800000	/* Incorrect Port Multiplier */
#define PORT_INTR_PRC	0x00400000	/* PhyRdy Change */
#define PORT_INTR_DMP	0x00000080	/* Device Mechanical Presence */
#define PORT_INTR_PC	0x00000040	/* Port Connect Change */
#define PORT_INTR_DP	0x00000020	/* Descriptor Processed */
#define PORT_INTR_UFIS	0x00000010	/* Unknown FIS */
#define PORT_INTR_SDB	0x00000008	/* Set Device Bits FIS */
#define PORT_INTR_DMAS	0x00000004	/* DMA Setup FIS */
#define PORT_INTR_PIOS	0x00000002	/* PIO Setup FIS */
#define PORT_INTR_DHR	0x00000001	/* Device to Host Register FIS */

/* Port Command Register */
#define PORT_CMD_ICC	0xf0000000	/* Interface Communication Control */
# define PORT_DEVSLEEP	0x80000000
# define PORT_SLUMBER	0x60000000
# define PORT_PARTIAL	0x20000000
# define PORT_ACTIVE	0x10000000
# define PORT_IDLE	0x00000000
#define PORT_CMD_ASP	0x08000000	/* Aggressive Slumber/Partial */
#define PORT_CMD_ALPE	0x04000000	/* Aggressive Link Power Management */
#define PORT_CMD_DLAE	0x02000000	/* Drive LED on ATAPI Enable */
#define PORT_CMD_ATAPI	0x01000000	/* Device is ATAPI */
#define PORT_CMD_APSTE	0x00800000	/* Auto Partial to Slumber Transition */
#define PORT_CMD_FBSCP	0x00400000	/* FIS-based Switching Capable Port */
#define PORT_CMD_ESP	0x00200000	/* External SATA Port */
#define PORT_CMD_CPD	0x00100000	/* Cold Presence Detection Support */
#define PORT_CMD_MPSP	0x00080000	/* Mechanical Presence Switch Support */
#define PORT_CMD_HPCP	0x00040000	/* Hot Plug Capable Port */
#define PORT_CMD_PMA	0x00020000	/* Port Multiplier Attached */
#define PORT_CMD_CPS	0x00010000	/* Cold Presence State */
#define PORT_CMD_CR	0x00008000	/* Command List Running */
#define PORT_CMD_FR	0x00004000	/* FIS Receive Running */
#define PORT_CMD_MPSS	0x00002000	/* Mechanical Presence Switch State */
#define PORT_CMD_CCS	0x00001f00	/* Current Command Slot */
#define PORT_CMD_FRE	0x00000010	/* FIS Receive Enable */
#define PORT_CMD_CLO	0x00000008	/* Command List Override */
#define PORT_CMD_POD	0x00000004	/* Power On Device */
#define PORT_CMD_SUD	0x00000002	/* Spin-Up Device */
#define PORT_CMD_START	0x00000001	/* Start */

#define PORT_CMD_CCS_SHIFT	8	/* Current Command Slot bit position */
static inline uint ahci_port_ccs(volatile struct ahci_hba_port *port)
{
	return (port->cmd & PORT_CMD_CCS) >> PORT_CMD_CCS_SHIFT;
}

/* Task file data register - Read Only */
#define PORT_TFD_ERROR_MASK	0x0000ff00
#define PORT_TFD_STATUS_MASK	0x000000ff
#define PORT_TFD_ERROR_SHIFT	8
#define PORT_TFD_STATUS_SHIFT	0
static inline uint ahci_port_tfd_error(volatile struct ahci_hba_port *port)
{
	return (port->tfd & PORT_TFD_ERROR_MASK) >> PORT_TFD_ERROR_SHIFT;
}

/* The status field reflects that of the Device-to-Host FIS */
static inline uint ahci_port_tfd_status(volatile struct ahci_hba_port *port)
{
	return (port->tfd & PORT_TFD_STATUS_MASK) >> PORT_TFD_STATUS_SHIFT;
}

/* Port SStatus register */
#define PORT_SSTAT_DET_MASK	0x0000000f
# define PORT_SSTAT_DET_NONE	0x00000000
# define PORT_SSTAT_DET_PRESENT	0x00000001
# define PORT_SSTAT_DET_CONNECT	0x00000003
# define PORT_SSTAT_DET_OFFLINE	0x00000004
#define PORT_SSTAT_IPM_MASK	0x00000f00
# define PORT_SSTAT_IPM_ACTIVE	0x00000100
# define PORT_SSTAT_IPM_PARTIAL	0x00000200
# define PORT_SSTAT_IPM_SLUMBER	0x00000600
# define PORT_SSTAT_IPM_DEVSLP	0x00000800

/* Port SControl register */
#define PORT_SCNTL_DET_MASK	0x0000000f	/* Device initialization */
# define PORT_SCNTL_DET_NONE	0x00000000
# define PORT_SCNTL_DET_RESET	0x00000001
# define PORT_SCNTL_DET_OFFLINE	0x00000004
static inline void ahci_port_reset(volatile struct ahci_hba_port *port)
{
	port->scontrol = (port->scontrol & ~PORT_SCNTL_DET_MASK) |
	    PORT_SCNTL_DET_RESET;
	/* Delay for some time */
	delay(1);
	port->scontrol = (port->scontrol & ~PORT_SCNTL_DET_MASK) |
	    PORT_SCNTL_DET_NONE;
}

/* 
 * HBA Generic Host Control structure
 */
struct ahci_hba {
	union {
		struct {
			uint	cap;		/* Capability */
			uint	ghc;		/* Global Host Control */
			uint	intr_stat;	/* Interrupt Status */
			uint	ports_impl;	/* Ports Implemented */
			uint	version;	/* Version */
			uint	ccc_ctl;	/* CCC Control */
			uint	ccc_ports;	/* CCC Ports */
			uint	em_loc;		/* EM Location */
			uint	em_ctl;		/* EM Control */
			uint	cap2;		/* Extended Capability */
			uint	bohc;		/* BIOS/OS Handoff Control */
		};
		unsigned char	data[0x100];
	};
	struct ahci_hba_port	port[32];
};

/*
 * Capability Register Bitmasks
 *
 * Abbreviations:
 * NCQ		- Native Command Queuing
 */
#define HBA_CAP_S64A	0x80000000	/* 64-bit addressing support */
#define HBA_CAP_SNCQ	0x40000000	/* NCQ Support */
#define HBA_CAP_SSNTF	0x20000000	/* SNotification Register support */
#define HBA_CAP_SMPS	0x10000000	/* Mechanical Presence Switch support */
#define HBA_CAP_SSS	0x08000000	/* Staggered Spinup Support */
#define HBA_CAP_SALP	0x04000000	/* Aggressive Link Power Support */
#define HBA_CAP_SAL	0x02000000	/* Activity LED Support */
#define HBA_CAP_SCLO	0x01000000	/* Command List Override Support */
#define HBA_CAP_ISSMASK	0x00f00000	/* Interface Speed Support Bitmask */
# define HBA_SPEED_GEN1	0x00100000	/* Gen 1 (1.5Gbps) */
# define HBA_SPEED_GEN2	0x00200000	/* Gen 2 (3Gbps) */
# define HBA_SPEED_GEN3 0x00300000	/* Gen 3 (6Gbps) */
#define HBA_CAP_SAM	0x00040000	/* AHCI-only */
#define HBA_CAP_SPM	0x00020000	/* Ports Multiplier Support */
#define HBA_CAP_FBSS	0x00010000	/* FIS-based Switching Support */
#define HBA_CAP_PMD	0x00008000	/* PIO Multiple Data Request Block */
#define HBA_CAP_SSC	0x00004000	/* Slumber State Capable */
#define HBA_CAP_PSC	0x00002000	/* Partial State Capable */
#define HBA_CAP_SLOTS	0x00001f00	/* Number of command slots bitmask */
# define HBA_CAP_SLOTS_SHIFT	8	/* Bit shift, maybe redundant */
#define HBA_CAP_CCCS	0x00000080	/* CCC Support */
#define HBA_CAP_EMS	0x00000040	/* EM Support */
#define HBA_CAP_SXS	0x00000020	/* External SATA Support */
#define HBA_CAP_PORTS	0x0000001f	/* Number of ports bitmask */
# define HBA_CAP_PORTS_SHIFT	0	/* Bit shift, maybe redundant */

static inline unsigned int ahci_hba_slots(volatile struct ahci_hba *hba)
{
	return ((hba->cap & HBA_CAP_SLOTS) >> HBA_CAP_SLOTS_SHIFT) + 1;
}

static inline unsigned int ahci_hba_ports(volatile struct ahci_hba *hba)
{
	return ((hba->cap & HBA_CAP_PORTS) >> HBA_CAP_PORTS_SHIFT) + 1;
}

static inline bool ahci_support_ccc(volatile struct ahci_hba *hba)
{
	return !!(hba->cap & HBA_CAP_CCCS);
}

static inline bool ahci_support_em(volatile struct ahci_hba *hba)
{
	return !!(hba->cap & HBA_CAP_EMS);
}

/*
 * Global Host Control Bitmasks
 */
#define HBA_GHC_AE	0x80000000	/* AHCI Enable */
#define HBA_GHC_MRSM	0x00000004	/* MSI Revert to Single Message */
#define HBA_GHC_IE	0x00000002	/* Interrupt Enable */
#define HBA_GHC_HR	0x00000001	/* HBA Reset */

static inline void ahci_disable_intr(volatile struct ahci_hba *hba)
{
	hba->ghc &= ~HBA_GHC_IE;
}

static inline void ahci_enable_intr(volatile struct ahci_hba *hba)
{
	hba->ghc |= HBA_GHC_IE;
}

static inline void ahci_hba_reset(volatile struct ahci_hba *hba)
{
	hba->ghc |= HBA_GHC_HR;
	while (hba->ghc & HBA_GHC_HR)
		/* nothing */;
}

/*
 * Interrupt Status Register is merely a bitmap representing which port
 * generated an interrupt.
 *
 * Ports Implemented Register is another bitmap indicating available ports.
 *
 * Version Register... I don't care, mostly.
 */

/*
 * Command Completion Coalescing registers are manipulated via inline functions.
 */
#define HBA_CCC_CTL_TV_SHIFT	16
#define HBA_CCC_CTL_TV_MASK	0xffff0000
#define HBA_CCC_CTL_CC_SHIFT	8
#define HBA_CCC_CTL_CC_MASK	0x0000ff00
#define HBA_CCC_CTL_INT_SHIFT	3
#define HBA_CCC_CTL_INT_MASK	0x000000f8
#define HBA_CCC_CTL_ENABLE	0x00000001

static inline uint ahci_ccc_gettimeout(volatile struct ahci_hba *hba)
{
	if (!ahci_support_ccc(hba))
		return 0;
	return (hba->ccc_ctl & HBA_CCC_CTL_TV_MASK) >> HBA_CCC_CTL_TV_SHIFT;
}

static inline void ahci_ccc_settimeout(volatile struct ahci_hba *hba, uint tv)
{
	if (!ahci_support_ccc(hba))
		return;
	hba->ccc_ctl = (hba->ccc_ctl & ~HBA_CCC_CTL_TV_MASK) |
	    (tv << HBA_CCC_CTL_TV_SHIFT);
}

static inline uint ahci_ccc_getcc(volatile struct ahci_hba *hba)
{
	if (!ahci_support_ccc(hba))
		return 0;
	return (hba->ccc_ctl & HBA_CCC_CTL_CC_MASK) >> HBA_CCC_CTL_CC_SHIFT;
}

static inline void ahci_ccc_setcc(volatile struct ahci_hba *hba, uint cc)
{
	if (!ahci_support_ccc(hba))
		return;
	hba->ccc_ctl = (hba->ccc_ctl & ~HBA_CCC_CTL_CC_MASK) |
	    (cc << HBA_CCC_CTL_CC_SHIFT);
}

static inline uint ahci_ccc_getintr(volatile struct ahci_hba *hba)
{
	if (!ahci_support_ccc(hba))
		return 0;
	return (hba->ccc_ctl & HBA_CCC_CTL_INT_MASK) >> HBA_CCC_CTL_INT_SHIFT;
}

static inline void ahci_ccc_setintr(volatile struct ahci_hba *hba, uint intr)
{
	if (!ahci_support_ccc(hba))
		return;
	hba->ccc_ctl = (hba->ccc_ctl & ~HBA_CCC_CTL_INT_MASK) |
	    (intr << HBA_CCC_CTL_INT_SHIFT);
}

static inline void ahci_enable_ccc(volatile struct ahci_hba *hba)
{
	if (!ahci_support_ccc(hba))
		return;
	hba->ccc_ctl |= HBA_CCC_CTL_ENABLE;
}

static inline void ahci_disable_ccc(volatile struct ahci_hba *hba)
{
	if (!ahci_support_ccc(hba))
		return;
	hba->ccc_ctl &= ~HBA_CCC_CTL_ENABLE;
}

#endif
