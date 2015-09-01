/*
 * Copyright (C) 2015 David Gao <davidgao1001@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

.globl preload_bootasm

.text
.arm

preload_bootasm:
	ldr	sp, =preload_vector
	movs	fp, sp
	bl	preload_bootmain

