/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <asm/asm.h>
#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <smp.h>

LEAF(_start)
	/* TODO: assembly startup code */
	mfc0	t0, CP0_STATUS
	or	t0, ST_KX|ST_CU0|ST_IM
	or	t0, ST_EXCM
	xor	t0, ST_EXCM
	mtc0	t0, CP0_STATUS

	mtc0	zero, CP0_WATCHLO
	mtc0	zero, CP0_WATCHHI

	LA	sp, cpu_info_stack
	ADDIU	sp, KSTACKSIZE
	ADDIU	sp, -32

	j	main

1:	j	1b
	nop
END(_start)
