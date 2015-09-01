/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <asm/mipsregs.h>
#include <drivers/rtc/rtc.h>

unsigned long cpu_freq;

#define NR_FREQ_SAMPLES	4

void probe_cpu_freq(void)
{
	unsigned int cnt, cnt_new, i, sec;
	cnt = read_c0_count();

	for (i = 0; i < NR_FREQ_SAMPLES + 1; ++i) {
		sec = rtc_getsecond();
		cnt = read_c0_count();
		do {
			cnt_new = read_c0_count();
		} while (rtc_getsecond() != sec);
		if (i != 0)
			cpu_freq += cnt_new - cnt;
	}

	cpu_freq /= 4;
}

void clock_init(void)
{
	rtc_init();
	probe_cpu_freq();
}

void setup_arch(void)
{
	/* Initialize hardware clocks */
	clock_init();
}
