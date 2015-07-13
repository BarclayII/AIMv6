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
	/* Locate the entrance */
	bal	locate
	move	v0, s0
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
	li	a0, 0x12345678
	bal	hexserial32
	ls	a0, __crlf
	bal	stringserial
	li	a0, 12345678
	bal	udecserial
	ls	a0, __crlf
	bal	stringserial
1:	nop
	j	1b
END(_start)

LEAF(locate)
	move	v0, ra
	addiu	v0, -8
	jr	ra
END(locate)

DEFINE_STRING(__test_serial, "\r\nSerial Initialization Success\r\n\0")
DEFINE_STRING(__crlf, "\r\n\0")
