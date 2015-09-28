/*
 * Copyright (C) 2015 David Gao <davidgao1001@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

.globl firmware_bootasm

.text
.arm

firmware_bootasm:
	ldr	sp, =firmware_vector
	movs	fp, sp
	bl	firmware_bootmain

