/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <config.h>
#include <printk.h>
#include <asm/setup.h>
#include <asm/mipsregs.h>
#include <drivers/clock/rtc.h>

/* CPU pipeline frequency */
unsigned long cpu_freq;

#define NR_FREQ_SAMPLES	4

void probe_count_freq(void)
{
	unsigned int cnt, cnt_new, i, sec;
	unsigned long count_inc;
	cnt = read_c0_count();
	count_inc = 0;

	for (i = 0; i < NR_FREQ_SAMPLES + 1; ++i) {
		sec = rtc_getsecond();
		cnt = read_c0_count();
		do {
			cnt_new = read_c0_count();
		} while (rtc_getsecond() == sec);
		if (i != 0)
			count_inc += cnt_new - cnt;
	}

	count_inc /= NR_FREQ_SAMPLES;

	/* 
	 * The rate of increasing COUNT register on MIPS really depends on
	 * implementation.
	 */
	cpu_freq = COUNT_TO_FREQ(count_inc);
}

void clock_init(void)
{
	printk("Initializing Real Time Clock (RTC)...\r\n");
	rtc_init();
	printk(
	    "Current time: %04d-%02d-%02d %02d:%02d:%02d\r\n",
	    rtc_getyear(),
	    rtc_getmonth(),
	    rtc_getday(),
	    rtc_gethour(),
	    rtc_getminute(),
	    rtc_getsecond()
	);

	printk("Determining CPU frequency...\r\n");
	probe_count_freq();
	printk("CPU frequency: %u\r\n", cpu_freq);
}

void setup_arch(void)
{
	/* Initialize hardware clocks */
	clock_init();
}
