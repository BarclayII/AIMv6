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
 * MC146818 - Morotola Real Time Clock (RTC) + RAM
 *
 * References:
 * http://www.loongnix.cn/dev/ftp/doc/02data%20sheet/loongson3a/SB/43009_sb7xx_rrg_pub_1.00.pdf
 */

#include <config.h>

#ifdef RTC_MC146818

#include <drivers/clock/mc146818.h>

void rtc_init(void)
{
	/*
	 * Set DV0 to enable access to century register, no interrupt,
	 * and enable oscillator.
	 */
	rtc_writereg(DS_REG_CTLA, DS_CTLA_DV0 | DS_CTLA_DV1);
	/* 24hr mode */
	rtc_writereg(DS_REG_CTLB, DS_CTLB_24);
}

unsigned char rtc_gettimereg(unsigned char r)
{
	return bcd_to_byte(rtc_readreg(r));
}

void rtc_settimereg(unsigned char r, unsigned char v)
{
	unsigned char ctlb = rtc_readreg(DS_REG_CTLB);
	rtc_writereg(DS_REG_CTLB, ctlb | DS_CTLB_SET);
	rtc_writereg(r, byte_to_bcd(v));
	rtc_writereg(DS_REG_CTLB, ctlb & ~DS_CTLB_SET);
}

#endif	/* RTC_MC146818 */
