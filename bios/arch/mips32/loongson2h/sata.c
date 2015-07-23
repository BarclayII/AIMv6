/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <stddef.h>
#include <asm/io.h>
#include <asm/mm/page.h>
#include <drivers/ata/ahci.h>
#include <drivers/serial/uart.h>

volatile struct ahci_hba *hba = (struct ahci_hba *)iomap(SATA_BASE);

unsigned int hba_ports_available = 0;

static inline void ahci_print_single_cap(const char *desc, unsigned int mask)
{
	uart_spin_printf("\t%30s: %s\r\n", desc, hba->cap & mask ? "YES" : "NO");
}

static void ahci_determine_speed(void)
{
	uart_spin_printf("\t%30s: ", "Generation");
	switch (hba->cap & HBA_CAP_ISSMASK) {
	case HBA_SPEED_GEN1:
		uart_spin_printf("Gen 1. (1.5Gbps)\r\n");
		break;
	case HBA_SPEED_GEN2:
		uart_spin_printf("Gen 2. (3Gbps)\r\n");
		break;
	case HBA_SPEED_GEN3:
		uart_spin_printf("Gen 3. (6Gbps)\r\n");
		break;
	}
}

void ahci_print_cap(volatile struct ahci_hba *hba)
{
	uart_spin_printf("SATA capability: %08x\r\n", hba->cap);

	ahci_print_single_cap("Support 64-bit addressing", HBA_CAP_S64A);
	ahci_print_single_cap("Native Command Queuing", HBA_CAP_SNCQ);
	ahci_print_single_cap("SNotification", HBA_CAP_SSNTF);
	ahci_print_single_cap("Mechanical Presence Switch", HBA_CAP_SMPS);
	ahci_print_single_cap("Staggered Spinup", HBA_CAP_SSS);
	ahci_print_single_cap("Aggressive Link Power", HBA_CAP_SALP);
	ahci_print_single_cap("Activity LED", HBA_CAP_SAL);
	ahci_print_single_cap("Command List Override", HBA_CAP_SCLO);
	ahci_determine_speed();
	ahci_print_single_cap("AHCI only", HBA_CAP_SAM);
	ahci_print_single_cap("Ports Multiplier", HBA_CAP_SPM);
	ahci_print_single_cap("FIS-based Switching", HBA_CAP_FBSS);
	ahci_print_single_cap("PIO Multiple Data Request", HBA_CAP_PMD);
	ahci_print_single_cap("Slumber", HBA_CAP_SSC);
	ahci_print_single_cap("Partial", HBA_CAP_PSC);
	uart_spin_printf("\t%30s: %d\r\n", "Slots", ahci_hba_slots(hba));
	ahci_print_single_cap("Command Completion Coalescing", HBA_CAP_CCCS);
	ahci_print_single_cap("Enclosure Management", HBA_CAP_EMS);
	ahci_print_single_cap("External SATA", HBA_CAP_SXS);
	uart_spin_printf("\t%30s: %d\r\n", "Ports", ahci_hba_ports(hba));
}

void ahci_print_ports(volatile struct ahci_hba *hba)
{
	unsigned int i;
	uart_spin_printf("Ports available:");
	for (i = 0; i < 32; ++i) {
		if (hba->ports_impl & (1 << i))
			uart_spin_printf(" %d", i);
	}
	uart_spin_printf("\r\n");
}

void sata_init(void)
{
	int port;

	uart_spin_printf("Resetting controller...\r\n");
	ahci_hba_reset(hba);
	uart_spin_printf("Reset controller\r\n");
	ahci_disable_intr(hba);
	ahci_print_cap(hba);
	ahci_print_ports(hba);

	/* Disable Command Completion Coalescing */
	ahci_disable_ccc(hba);

	for (port = 0; port < 32; ++port)
		if ((hba->ports_impl & (1 << port)) &&
		    (ahci_init_port(&hba->port[port], NULL) == 0)) {
			uart_spin_printf("Port %d is online\r\n", port);
			hba_ports_available |= 1 << port;
		}
}
