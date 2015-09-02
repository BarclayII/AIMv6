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

#ifdef SD_ZYNQ7000

#include <sys/types.h>
#include <asm/io.h>
#include <sleep.h>

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
	out16(SD_BASE + SD_ERR_INTR_STS_EN_OFFSET, SD_ERR_INTR_ALL);

	/* but disable all interrupt signals */
	out16(SD_BASE + SD_NORM_INTR_SIG_EN_OFFSET, 0x0);
	out16(SD_BASE + SD_ERR_INTR_SIG_EN_OFFSET, 0x0);

	/* set transfer mode: single block, DMA, read */
	tmp16 = SD_TM_DMA_EN | SD_TM_BLK_CNT_EN | SD_TM_DAT_DIR_SEL;
	out16(SD_BASE + SD_XFER_MODE_OFFSET, tmp16);

	/* set block size */
	out16(SD_BASE + SD_BLK_SIZE_OFFSET, 512);
}

/* add descriptions to a command */
u16 sd_frame_cmd(u16 cmd)
{
	switch (cmd) {
		case SD_CMD0:
		case SD_CMD4:
			cmd |= SD_RESP_NONE; break;
		case SD_ACMD6:
		case SD_CMD7:
		case SD_CMD10:
		case SD_CMD12:
		case SD_ACMD13:
		case SD_CMD16:
		case SD_ACMD42:
		case SD_CMD52:
		case SD_CMD55:
			cmd |= SD_RESP_R1; break;
		case SD_CMD17:
		case SD_CMD18:
		case SD_CMD23:
		case SD_ACMD23:
		case SD_CMD24:
		case SD_CMD25:
		case SD_ACMD51:
			cmd |= SD_RESP_R1 | SD_DAT_PRESENT; break;
		case SD_CMD5:
			cmd |= SD_RESP_R1B; break;
		case SD_CMD2:
		case SD_CMD9:
			cmd |= SD_RESP_R2; break;
		case SD_CMD1:
		case SD_ACMD41:
			cmd |= SD_RESP_R3; break;
		case SD_CMD3:
			cmd |= SD_RESP_R6; break;
		case SD_CMD6:
			cmd |= SD_RESP_R1B; break;
			/* for MMC card it is RESP_R1 | SD_DAT_PRESENT_SEL */
		case SD_CMD8:
			cmd |= SD_RESP_R1 | SD_DAT_PRESENT; break;
			/* for MMC card it is RESP_R1 */
	}
	return cmd & 0x3FFF;
}

/*
 * really send command
 * 0 = good
 * 1 = command inhibited
 * 2 = command has data but data is inhibited
 * 3 = controller reported error
 */
int sd_spin_send_cmd(u16 cmd, u16 count, u32 arg)
{
	u32 state;
	u16 result;
	/* frame the command */
	cmd = sd_frame_cmd(cmd);
	/* do a state check */
	state = in32(SD_BASE + SD_PRES_STATE_OFFSET);
	if (state & SD_PSR_INHIBIT_CMD) return 1;
	if ((state & SD_PSR_INHIBIT_DAT) && (cmd & SD_DAT_PRESENT)) return 2;
	/* write block count */
	out16(SD_BASE + SD_BLK_CNT_OFFSET, count);
	out16(SD_BASE + SD_TIMEOUT_CTRL_OFFSET, 0xE);
	/* write argument */
	out32(SD_BASE + SD_ARGMT_OFFSET, arg);
	out16(SD_BASE + SD_NORM_INTR_STS_OFFSET, SD_NORM_INTR_ALL);
	out16(SD_BASE + SD_ERR_INTR_STS_OFFSET, SD_ERR_INTR_ALL);
	/* write command */
	out16(SD_BASE + SD_CMD_OFFSET, cmd);
	/* wait for result */
	do {
		result = in16(SD_BASE + SD_NORM_INTR_STS_OFFSET);
		if (result & SD_INTR_ERR) return 3;
		/* We don't read error states, and we dont't clear them. */
	} while(!(result & SD_INTR_CC));
	/* Clear */
	out16(SD_BASE + SD_NORM_INTR_STS_OFFSET, SD_INTR_CC);
	return 0;
}

/*
 * initialize a card
 * 0 = good
 * 1 = no card
 */
int sd_spin_init_card()
{
	u32 state;
	/* check card */
	state = in32(SD_BASE + SD_PRES_STATE_OFFSET);
	if (!(state & SD_PSR_CARD_INSRT)) return 1;
	/* wait. Xilinx does so. Unexplained. */
	usleep(2000);
	return 0;
}

#endif /* SD_ZYNQ7000 */
