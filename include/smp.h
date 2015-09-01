/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _SMP_H
#define _SMP_H

#include <config.h>
#include <asm/smp.h>

#define KSTACKSIZE	8192

#ifndef __ASSEMBLER__

/* per-CPU info */
struct cpuinfo {
	unsigned int	cpuno;		/* CPU # */
	unsigned int	ticks;		/* # of ticks - for clock interrupt */
};

union cpu_info_stack {
	struct cpuinfo	cpuinfo;
	unsigned char	stack[KSTACKSIZE];
};

extern union cpu_info_stack cpu_info_stack[];

#define current_cpuinfo	(cpu_info_stack[cpuid()].cpuinfo)

#endif	/* !__ASSEMBLER__ */

#endif
