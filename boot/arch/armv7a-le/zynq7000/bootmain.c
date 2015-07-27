/*
 * Copyright (C) 2015 David Gao <davidgao1001@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <config.h>

#define PRELOAD_VECTOR_BASE	0x20000

void (*uart_init)(void) = (void *)(PRELOAD_VECTOR_BASE + 0);
void (*uart_spin_puts)(const char *) = (void *)(PRELOAD_VECTOR_BASE + 4);

void bootmain(void)
{
	//uart_init();
	uart_spin_puts("Hello!\r\n");
	while(1);
}

