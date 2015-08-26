/*
 * Copyright (C) 2015 David Gao <davidgao1001@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <config.h>
#include <sys/types.h>
#include <asm/io.h>

#ifdef SD_ZYNQ7000

#include <drivers/sd/sd-zynq7000.h>

/*
 * Initialize the controller.
 * This can (almost) only be done in polling mode.
 */
void sd_init()
{
	u16 	tmp16;
	u8	tmp8;
	/* reset */
	out8(SD_BASE + SD_SW_RST_OFFSET, SD_SWRST_ALL_MASK);
	while (in8(SD_BASE + SD_SW_RST_OFFSET) & SD_SWRST_ALL_MASK);

	/* capabilities = in32(SD_BASE + SD_CAPS_OFFSET) */

	/* enable internal clock */
	tmp16 = SD_CC_SDCLK_FREQ_D128 | SD_CC_INT_CLK_EN;
	out16(SD_BASE + SD_CLK_CTRL_OFFSET, tmp16);
	while (!(in16(SD_BASE + SD_CLK_CTRL_OFFSET) & SD_CC_INT_CLK_STABLE));

	/* enable SD clock */
	tmp16 = in16(SD_BASE + SD_CLK_CTRL_OFFSET) | SD_CC_SD_CLK_EN;
	out16(SD_BASE + SD_CLK_CTRL_OFFSET, tmp16);
	
	/* enable bus power */
	tmp8 = SD_PC_BUS_VSEL_3V3 | SD_PC_BUS_PWR;
	out8(SD_BASE + SD_POWER_CTRL_OFFSET, tmp8);
	out8(SD_BASE + SD_HOST_CTRL1_OFFSET, SD_HC_DMA_SDMA);
	/*
	 * Xilinx's driver uses ADMA2 by default, we use single-operation
	 * DMA to avoid putting descriptors in memory.
	 */

	/* enable interrupt status except card */
	tmp16 = SD_NORM_INTR_ALL & (~SD_INTR_CARD);
	out16(SD_BASE + SD_NORM_INTR_STS_EN_OFFSET, tmp16);
	out16(SD_BASE + SD_ERR_INTR_STS_EN_OFFSET, SD_ERROR_INTR_ALL);

	/* but disable all interrupt signals */
	out16(SD_BASE + SD_NORM_INTR_SIG_EN_OFFSET, 0x0);
	out16(SD_BASE + SD_ERR_INTR_SIG_EN_OFFSET, 0x0);

	/* set transfer mode: single block, DMA, read */
	tmp16 = SD_TM_DMA_EN | SD_TM_BLK_CNT_EN | SD_TM_DAT_DIR_SEL;
	out16(SD_BASE + SD_XFER_MODE_OFFSET, tmp16);

	/* set block size */
	out16(SD_BASE + SD_BLK_SIZE_OFFSET, 512);
}

#endif /* SD_ZYNQ7000 */
