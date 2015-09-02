/*
 * Copyright (C) 2015 David Gao <davidgao1001@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _DRIVERS_CLOCK_GTC_A9MPCORE
#define _DRIVERS_CLOCK_GTC_A9MPCORE

#include <config.h>

#define GTC_OFFSET	0x200

#ifdef KERNEL
#define GTC_BASE	(mpcore_base + GTC_OFFSET)
#else /* KERNEL */
#define GTC_BASE	(MPCORE_PHYSBASE + GTC_OFFSET)
#endif /* KERNEL */

u64 gtc_get_time();

/*
 * ticks per second.
 * Zynq7000 is an APSoC and it's got a LOT of clocks.
 * GTC's freqency is half of core frequency.
 * core freqency is 6x or 8x ARM PLL
 * but ARM PLL itself is programmable ...
 * we set a LARGE value here to make sure sleep() and usleep() wait
 * long enough.
 * value here is 400M, means core run at 800MHz.
 */
#define GTC_TPUS 400l
#define GTC_TPS	(GTC_TPUS * 1000000l)
/* avoid division whenever possible.
 * DO NOT LET OPTIMIZATION DECIDE THIS BEHAVIOR!
 */

/* register offset */
#define GTC_COUNTER_LO_OFFSET		0x00
#define GTC_COUNTER_HI_OFFSET		0x04
#define GTC_CTRL_OFFSET			0x08
#define GTC_INT_OFFSET			0x0C
#define GTC_COMPARATOR_LO_OFFSET	0x10
#define GTC_COMPARATOR_HI_OFFSET	0x14
#define GTC_INCREMENT_OFFSET		0x18

#endif /* _DRIVERS_CLOCK_GTC_A9MPCORE */
