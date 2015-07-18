
#include <asm/prim.h>
#include <drivers/ata/ahci_hba.h>
#include <drivers/ata/ahci_fis.h>
#include <drivers/serial/uart16550.h>

volatile struct ahci_hba *hba = (struct ahci_hba *)iomap(SATA_BASE);

static inline void sata_print_single_cap(const char *desc, unsigned int mask)
{
	uart_spin_printf("\t%30s: %s\r\n", desc, hba->cap & mask ? "YES" : "NO");
}

static void sata_determine_speed(void)
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

void sata_print_cap(void)
{
	uart_spin_printf("SATA capability: %08x\r\n", hba->cap);

	sata_print_single_cap("Support 64-bit addressing", HBA_CAP_S64A);
	sata_print_single_cap("Native Command Queuing", HBA_CAP_SNCQ);
	sata_print_single_cap("SNotification", HBA_CAP_SSNTF);
	sata_print_single_cap("Mechanical Presence Switch", HBA_CAP_SMPS);
	sata_print_single_cap("Staggered Spinup", HBA_CAP_SSS);
	sata_print_single_cap("Aggressive Link Power", HBA_CAP_SALP);
	sata_print_single_cap("Activity LED", HBA_CAP_SAL);
	sata_print_single_cap("Command List Override", HBA_CAP_SCLO);
	sata_determine_speed();
	sata_print_single_cap("AHCI only", HBA_CAP_SAM);
	sata_print_single_cap("Ports Multiplier", HBA_CAP_SPM);
	sata_print_single_cap("FIS-based Switching", HBA_CAP_FBSS);
	sata_print_single_cap("PIO Multiple Data Request", HBA_CAP_PMD);
	sata_print_single_cap("Slumber", HBA_CAP_SSC);
	sata_print_single_cap("Partial", HBA_CAP_PSC);
	uart_spin_printf("\t%30s: %d\r\n", "Slots", ahci_hba_slots(hba));
	sata_print_single_cap("Command Completion Coalescing", HBA_CAP_CCCS);
	sata_print_single_cap("Enclosure Management", HBA_CAP_EMS);
	sata_print_single_cap("External SATA", HBA_CAP_SXS);
	uart_spin_printf("\t%30s: %d\r\n", "Ports", ahci_hba_ports(hba));
}
