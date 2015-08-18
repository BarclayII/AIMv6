/*
 * Copyright (C) 2015 David Gao <davidgao1001@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#define _DTB_C

#include <sys/types.h>
#include <config.h>
#include <drivers/misc/dtb-zynq7000.h>

u32	ocm_base		= OCM_PHYSBASE_LO;
u32	ddr_base		= DDR_PHYSBASE_SCU;
u32	axi_gp0_base		= AXI_GP0_PHYSBASE;
u32	axi_gp1_base		= AXI_GP1_PHYSBASE;
u32	qspi_linear_base	= QSPI_LINEAR_PHYSBASE;

u32	ddr_end			= DDR_PHYSEND;

u32	uart0_base		= UART0_PHYSBASE;
u32	uart1_base		= UART1_PHYSBASE;
u32	usb0_base		= USB0_PHYSBASE;
u32	usb1_base		= USB1_PHYSBASE;
u32	i2c0_base		= I2C0_PHYSBASE;
u32	i2c1_base		= I2C1_PHYSBASE;
u32	spi0_base		= SPI0_PHYSBASE;
u32	spi1_base		= SPI1_PHYSBASE;
u32	can0_base		= CAN0_PHYSBASE;
u32	can1_base		= CAN1_PHYSBASE;
u32	gpio_base		= GPIO_PHYSBASE;
u32	gem0_base		= GEM0_PHYSBASE;
u32	gem1_base		= GEM1_PHYSBASE;
u32	qspi_base		= QSPI_PHYSBASE;
u32	smcc_base		= SMCC_PHYSBASE;

u32	sd0_base		= SD0_PHYSBASE;
u32	sd1_base		= SD1_PHYSBASE;

u32	smc_nand_base		= SMC_NAND_PHYSBASE;
u32	smc_sram0_base		= SMC_SRAM0_PHYSBASE;
u32	smc_sram1_base		= SMC_SRAM1_PHYSBASE;

u32	slcr_base		= SLCR_PHYSBASE;
u32	ttc0_base		= TTC0_PHYSBASE;
u32	ttc1_base		= TTC1_PHYSBASE;
u32	dmac0_s_base		= DMAC0_S_PHYSBASE;
u32	dmac0_ns_base		= DMAC0_NS_PHYSBASE;
u32	swdt_base		= SWDT_PHYSBASE;
u32	ddrc_base		= DDRC_PHYSBASE;
u32	devcfg_base		= DEVCFG_PHYSBASE;
u32	axi_hp0_base		= AXI_HP0_PHYSBASE;
u32	axi_hp1_base		= AXI_HP1_PHYSBASE;
u32	axi_hp2_base		= AXI_HP2_PHYSBASE;
u32	axi_hp3_base		= AXI_HP3_PHYSBASE;
u32	ocmc_base		= OCMC_PHYSBASE;

u32	debug_dap_rom_base	= DEBUG_DAP_ROM_PHYSBASE;
u32	debug_etb_base		= DEBUG_ETB_PHYSBASE;
u32	debug_cti_etb_tpiu_base	= DEBUG_CTI_ETB_TPIU_PHYSBASE;
u32	debug_tpiu_base		= DEBUG_TPIU_PHYSBASE;
u32	debug_funnel_base	= DEBUG_FUNNEL_PHYSBASE;
u32	debug_itm_base		= DEBUG_ITM_PHYSBASE;
u32	debug_cti_ftm_base	= DEBUG_CTI_FTM_PHYSBASE;
u32	debug_ftm_base		= DEBUG_FTM_PHYSBASE;

u32	debug_cpu_pmu0_base	= DEBUG_CPU_PMU0_PHYSBASE;
u32	debug_cpu_pmu1_base	= DEBUG_CPU_PMU1_PHYSBASE;
u32	debug_cpu_cti0_base	= DEBUG_CPU_CTI0_PHYSBASE;
u32	debug_cpu_cti1_base	= DEBUG_CPU_CTI1_PHYSBASE;
u32	debug_cpu_ptm0_base	= DEBUG_CPU_PTM0_PHYSBASE;
u32	debug_cpu_ptm1_base	= DEBUG_CPU_PTM1_PHYSBASE;

u32	gpv_trustzone_base	= GPV_TRUSTZONE_PHYSBASE;
u32	gpv_qos301_cpu_base	= GPV_QOS301_CPU_PHYSBASE;
u32	gpv_qos301_dmac_base	= GPV_QOS301_DMAC_PHYSBASE;
u32	gpv_qos301_iou_base	= GPV_QOS301_IOU_PHYSBASE;

u32	mpcore_base		= MPCORE_PHYSBASE;
u32	l2cache_base		= L2CACHE_PHYSBASE;
