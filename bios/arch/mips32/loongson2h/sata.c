
#include <asm/prim.h>
#include <drivers/ata/ahci_hba.h>
#include <drivers/ata/ahci_fis.h>
#include <drivers/serial/uart.h>

volatile struct ahci_hba *hba = (struct ahci_hba *)iomap(SATA_BASE);

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
	ahci_disable_intr(hba);
	ahci_print_cap(hba);
	ahci_print_ports(hba);

	/* Disable Command Completion Coalescing */
	ahci_disable_ccc(hba);
}
