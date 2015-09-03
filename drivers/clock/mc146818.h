/*
 * Copyright (c) 2002 Opsycon AB  (www.opsycon.se / www.opsycon.com)
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Opsycon AB, Sweden.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#ifndef _DRIVERS_CLOCK_MC146818_H
#define _DRIVERS_CLOCK_MC146818_H

/*
 *   Register definitions for Dallas Semiconductor DS17285/287 clock chips.
 */

#define	DS_REG_SEC	0x00		/* Seconds reg */
#define	DS_REG_ASEC	0x01		/* Alarm Seconds reg */
#define	DS_REG_MIN	0x02		/* Minutes reg */
#define	DS_REG_AMIN	0x03		/* Alarm Minutes reg */
#define	DS_REG_HOUR	0x04		/* Hours reg */
#define	DS_REG_AHOUR	0x05		/* Alarm Hours reg */
#define	DS_REG_WDAY	0x06		/* Day of week reg */
#define	DS_REG_DATE	0x07		/* Day of month reg */
#define	DS_REG_MONTH	0x08		/* Month reg */
#define	DS_REG_YEAR	0x09		/* Year reg */

#define	DS_REG_CTLA	0x0a		/* Control reg A */
#define	DS_REG_CTLB	0x0b		/* Control reg B */
#define	DS_REG_CTLC	0x0c		/* Control reg C */
#define	DS_REG_CTLD	0x0d		/* Control reg D */


#define	DS_MONTH_E32K	0x40		/* Enable 32Khz out when 0 */
#define	DS_MONTH_EOSC	0x80		/* Enable oscilator when 0 */

#define	DS_CTLA_RS0	0x01		/* Rate Select */
#define	DS_CTLA_RS1	0x02		/* Rate Select */
#define	DS_CTLA_RS2	0x04		/* Rate Select */
#define	DS_CTLA_RS3	0x08		/* Rate Select */
#define	DS_CTLA_DV0	0x10		/* Bank Select */
#define	DS_CTLA_DV1	0x20		/* Osc. Enable */
#define	DS_CTLA_DV2	0x40		/* Countdown Chain */
#define	DS_CTLA_UIP	0x80		/* Update In Progress flag 1 */

#define DS_CTLB_DSE	0x01		/* Daylight savings enable */
#define	DS_CTLB_24	0x02		/* 24hr enable */
#define	DS_CTLB_DM	0x04		/* Data mode */
#define	DS_CTLB_SQWE	0x08		/* Square wave enable */
#define	DS_CTLB_UIE	0x10		/* Update ended interrupt enable */
#define	DS_CTLB_AIE	0x20		/* Alarm interrupt enable */
#define	DS_CTLB_PIE	0x40		/* Periodic interrupt enable */
#define	DS_CTLB_SET	0x80		/* Set registers enable */

/*
 *  Registers in Bank 1
 */

#define DS_REG_CENT	0x48		/* Century reg */

#define	DS_EXRAM_LSB	0x50		/* Extended RAM LSB addr */
#define	DS_EXRAM_MSB	0x51		/* Extended RAM MSB addr */
#define	DS_EXRAM_DATA	0x53		/* Extended RAM Data port */

#define PCI_RTC_ADDR_REG	0x70	/* RTC address PCI register */
#define PCI_RTC_DATA_REG	0x71	/* RTC data PCI register */

#include <asm/pci.h>

/*
 * readreg() and writereg() are for writing *raw* values.  For time
 * registers, that means to write value in BCD form.
 */
static inline unsigned char rtc_readreg(unsigned char reg)
{
	outb(PCI_RTC_ADDR_REG, reg);
	return inb(PCI_RTC_DATA_REG);
}

static inline void rtc_writereg(unsigned char reg, unsigned char val)
{
	outb(PCI_RTC_ADDR_REG, reg);
	outb(PCI_RTC_DATA_REG, val);
}

void rtc_init(void);

static inline unsigned char bcd_to_byte(unsigned char bcd)
{
	return ((bcd & 0xF) + ((bcd >> 4) & 0xF) * 10);
}

static inline unsigned char byte_to_bcd(unsigned char byte)
{
	return ((((byte / 10) % 10) << 4) + (byte % 10));
}

/*
 * gettimereg() reads time registers and converts them from BCD to integer.
 * settimereg() converts the value to BCD and writes to time registers.
 */
unsigned char rtc_gettimereg(unsigned char reg);
void rtc_settimereg(unsigned char reg, unsigned char val);

#define rtc_getcentury()	rtc_gettimereg(DS_REG_CENT)
#define rtc_getyearshort()	rtc_gettimereg(DS_REG_YEAR)
#define rtc_getmonth()		rtc_gettimereg(DS_REG_MONTH)
#define rtc_getday()		rtc_gettimereg(DS_REG_DATE)
#define rtc_getdayofweek()	rtc_gettimereg(DS_REG_WDAY)
#define rtc_gethour()		rtc_gettimereg(DS_REG_HOUR)
#define rtc_getminute()		rtc_gettimereg(DS_REG_MIN)
#define rtc_getsecond()		rtc_gettimereg(DS_REG_SEC)

#define rtc_getyear()	\
	((unsigned int)rtc_getcentury() * 100 + rtc_getyearshort())

#define rtc_setcentury(v)	rtc_settimereg(DS_REG_CENT, v)
#define rtc_setyearshort(v)	rtc_settimereg(DS_REG_YEAR, v)
#define rtc_setmonth(v)		rtc_settimereg(DS_REG_MONTH, v)
#define rtc_setday(v)		rtc_settimereg(DS_REG_DATE, v)
#define rtc_setdayofweek(v)	rtc_settimereg(DS_REG_WDAY, v)
#define rtc_sethour(v)		rtc_settimereg(DS_REG_HOUR, v)
#define rtc_setminute(v)	rtc_settimereg(DS_REG_MIN, v)
#define rtc_setsecond(v)	rtc_settimereg(DS_REG_SEC, v)

#define rtc_setyear(v)	\
	do {	\
		rtc_setcentury((v) / 100);	\
		rtc_setyearshort((v) % 100);	\
	} while (0)

#endif /* _DRIVERS_RTC_MC146818_H */

