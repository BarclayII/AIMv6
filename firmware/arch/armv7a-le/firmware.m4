changecom(`/*', `*/')
changequote([,])
/*
 * Copyright (C) 2015 David Gao <davidgao1001@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

sinclude([board.m4])
sinclude([chip.m4])

define([preload_size], [1M])
[/* preload_size =] preload_size [*/]

MEMORY
{
	FIRMWARE (rwx) :
		ORIGIN = ram_size - preload_size,
		LENGTH = preload_size
}

SECTIONS
{
	.vector : {
		*vector.o(.text)
	}
	.text : {
		*(.text)
	}
	.rodata : {
		*(.rodata)
	}
	.data : {
		*(.data)
	}
	.bss : {
		*(.bss)
	}
}

ENTRY(firmware_vector)
