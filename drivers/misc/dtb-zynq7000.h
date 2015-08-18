/*
 * Copyright (C) 2015 David Gao <davidgao1001@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _DTB_H
#define _DTB_H

#include <sys/types.h>

/* PHYSICAL BASE ADDRESS AS MACROS */
/* See Zynq7000 APSoC TRM (Xilinx UG585) for details. */
#define OCM_PHYSBASE_LO			0x00000000	/* size=256K */
#define DDR_PHYSBASE_SCU		0x00000000
#define DDR_PHYSBASE_NOSCU		0x00100000
#define AXI_GP0_PHYSBASE		0x40000000	/* size=1G */
#define AXI_GP1_PHYSBASE		0x80000000	/* size=1G */

/* FIXME DDR_PHYSEND is from Zedboard and not Zynq7000 */
#define DDR_PHYSEND			0x20000000	/* 512M */

#define UART0_PHYSBASE			0xE0000000
#define UART1_PHYSBASE			0xE0001000
#define USB0_PHYSBASE			0xE0002000
#define USB1_PHYSBASE			0xE0003000
#define I2C0_PHYSBASE			0xE0004000
#define I2C1_PHYSBASE			0xE0005000
#define SPI0_PHYSBASE			0xE0006000
#define SPI1_PHYSBASE			0xE0007000
#define CAN0_PHYSBASE			0xE0008000
#define CAN1_PHYSBASE			0xE0009000
#define GPIO_PHYSBASE			0xE000A000
#define GEM0_PHYSBASE			0xE000B000
#define GEM1_PHYSBASE			0xE000C000
#define QSPI_PHYSBASE			0xE000D000
#define SMCC_PHYSBASE			0xE000E000

#define SD0_PHYSBASE			0xE0100000
#define SD1_PHYSBASE			0xE0101000

#define SMC_NAND_PHYSBASE		0xE1000000
#define SMC_SRAM0_PHYSBASE		0xE2000000
#define SMC_SRAM1_PHYSBASE		0xE4000000

#define SLCR_PHYSBASE			0xF8000000
#define TTC0_PHYSBASE			0xF8001000
#define TTC1_PHYSBASE			0xF8002000
#define DMAC0_S_PHYSBASE		0xF8003000
#define DMAC0_NS_PHYSBASE		0xF8004000
#define SWDT_PHYSBASE			0xF8005000
#define DDRC_PHYSBASE			0xF8006000
#define DEVCFG_PHYSBASE			0xF8007000
#define AXI_HP0_PHYSBASE		0xF8008000
#define AXI_HP1_PHYSBASE		0xF8009000
#define AXI_HP2_PHYSBASE		0xF800A000
#define AXI_HP3_PHYSBASE		0xF800B000
#define OCMC_PHYSBASE			0xF800C000

#define DEBUG_DAP_ROM_PHYSBASE		0xF8800000
#define DEBUG_ETB_PHYSBASE		0xF8801000
#define DEBUG_CTI_ETB_TPIU_PHYSBASE	0xF8802000
#define DEBUG_TPIU_PHYSBASE		0xF8803000
#define DEBUG_FUNNEL_PHYSBASE		0xF8804000
#define DEBUG_ITM_PHYSBASE		0xF8805000
#define DEBUG_CTI_FTM_PHYSBASE		0xF8809000
#define DEBUG_FTM_PHYSBASE		0xF880B000

#define DEBUG_CPU_PMU0_PHYSBASE		0xF8891000
#define DEBUG_CPU_PMU1_PHYSBASE		0xF8893000
#define DEBUG_CPU_CTI0_PHYSBASE		0xF8898000
#define DEBUG_CPU_CTI1_PHYSBASE		0xF8899000
#define DEBUG_CPU_PTM0_PHYSBASE		0xF889C000
#define DEBUG_CPU_PTM1_PHYSBASE		0xF889D000

#define GPV_TRUSTZONE_PHYSBASE		0xF8900000
#define GPV_QOS301_CPU_PHYSBASE		0xF8946000
#define GPV_QOS301_DMAC_PHYSBASE	0xF8947000
#define GPV_QOS301_IOU_PHYSBASE		0xF8948000

#define MPCORE_PHYSBASE			0xF8F00000	/* size=8K */
#define L2CACHE_PHYSBASE		0xF8F02000

#define QSPI_LINEAR_PHYSBASE		0xFC000000
#define OCM_PHYSBASE_HI			0xFFFC0000	/* size=256K */

#ifdef KERNEL
#ifndef _DTB_C

extern u32	ocm_base;
extern u32	ddr_base;
extern u32	axi_gp0_base;
extern u32	axi_gp1_base;
extern u32	qspi_linear_base;

extern u32	ddr_end;

extern u32	uart0_base;
extern u32	uart1_base;
extern u32	usb0_base;
extern u32	usb1_base;
extern u32	i2c0_base;
extern u32	i2c1_base;
extern u32	spi0_base;
extern u32	spi1_base;
extern u32	can0_base;
extern u32	can1_base;
extern u32	gpio_base;
extern u32	gem0_base;
extern u32	gem1_base;
extern u32	qspi_base;
extern u32	smcc_base;

extern u32	sd0_base;
extern u32	sd1_base;

extern u32	smc_nand_base;
extern u32	smc_sram0_base;
extern u32	smc_sram1_base;

extern u32	slcr_base;
extern u32	ttc0_base;
extern u32	ttc1_base;
extern u32	dmac0_s_base;
extern u32	dmac0_ns_base;
extern u32	swdt_base;
extern u32	ddrc_base;
extern u32	devcfg_base;
extern u32	axi_hp0_base;
extern u32	axi_hp1_base;
extern u32	axi_hp2_base;
extern u32	axi_hp3_base;
extern u32	ocmc_base;

extern u32	debug_dap_rom_base;
extern u32	debug_etb_base;
extern u32	debug_cti_etb_tpiu_base;
extern u32	debug_tpiu_base;
extern u32	debug_funnel_base;
extern u32	debug_itm_base;
extern u32	debug_cti_ftm_base;
extern u32	debug_ftm_base;

extern u32	debug_cpu_pmu0_base;
extern u32	debug_cpu_pmu1_base;
extern u32	debug_cpu_cti0_base;
extern u32	debug_cpu_cti1_base;
extern u32	debug_cpu_ptm0_base;
extern u32	debug_cpu_ptm1_base;

extern u32	gpv_trustzone_base;
extern u32	gpv_qos301_cpu_base;
extern u32	gpv_qos301_dmac_base;
extern u32	gpv_qos301_iou_base;

extern u32	mpcore_base;
extern u32	l2cache_base;

#endif /* _DTB_C */
#endif /* KERNEL */

#endif /* _DTB_H */
