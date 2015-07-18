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
#include <asm/asm.h>
#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <drivers/serial/uart16550.h>

#define STACKSIZE	0x80000

NESTED(_reset, 0, sp)
	/*
	 * Startup routine:
	 * 0. Setup registers and coprocessor registers
	 * 1. Initialize serial (by assembly, therefore it's inevitably
	 *    machine specific.
	 * 2. Init DDR (blank as currently this is a network bootstrapper)
	 * 3. Cache (ditto)
	 * 4. TLB (ditto)
	 * 5. Copy from ROM to RAM (ditto)
	 * 6. Build primitive exception handler
	 * 7. Probe SATA
	 * 8. Copy MBR to RAM
	 * 9. Execute MBR
	 */

	/*
	 * Reserved static registers:
	 * s0 - Entry address
	 * s1 - RAM program end address
	 */

	/* Locate the entrance */
	bal	locate
	move	s0, v0
	/* Start initialization */
	bal	_start
1:
	j	1b
END(_reset)

.org	0x300

NESTED(_start, 0, sp)
	/*
	 * Some registers are reserved for serial consoles until we jumped
	 * to bootmain().
	 * See boot/arch/mips32/loongson2h/early_serial.S for extra notes
	 * when writing assembly.
	 */
	bal	initserial
	ls	a0, __test_serial
	bal	stringserial

	/*
	 * Assume that Step 2 to Step 5 are done *magically*.
	 */
	bal	initddr
	bal	initcache
	bal	inittlb
	bal	copy_to_ram
	bal	initstack
	ls	a0, __bootmain_msg
	bal	stringserial
	j	bootmain
1:	nop
	j	1b
END(_start)

LEAF(locate)
	move	v0, ra
	addiu	v0, -8
	jr	ra
END(locate)

LEAF(copy_to_ram)
	/* If the code is already at RAM, do nothing */
	li	t0, 0xbfc00000
	ls	t1, __end__
	blt	s0, t0, 1f
	/* Copy the entire ROM to RAM */
	li	s1, 0x88c00000
2:	lw	a0, (s0)
	sw	a0, (s1)
	addiu	s0, 4
	addiu	s1, 4
	blt	s0, t1, 2b
	/* Jump to RAM */
	subu	t1, s1
	subu	ra, t1
	jr	ra
1:	move	s1, t1
	jr	ra
END(copy_to_ram)

LEAF(initstack)
	li	v0, STACKSIZE
	addu	sp, s1, v0
	jr	ra
END(initstack)

DEFINE_STRING(__test_serial, "\r\nSerial Initialization Success\r\n\0")
DEFINE_STRING(__bootmain_msg, "Entering bootmain()\r\n\0")
DEFINE_STRING(__crlf, "\r\n\0")
