/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _STDDEF_H
#define _STDDEF_H

#ifndef NULL
#define NULL	(void *)0
#endif

#ifndef BUFSIZ
#define BUFSIZ		1024
#endif

#define MEMBER_OFFSET(struct, member_name) \
	((unsigned long)&(((struct *)0)->member_name))

#define member_to_struct(addr, struct, member_name) \
	((struct *)((unsigned long)(addr) - MEMBER_OFFSET(struct, member_name)))

/*
 * An extremely inaccurate busy-wait loop for delaying @t milliseconds
 */
static inline void delay(unsigned long t)
{
	register unsigned long i;
	for (i = 0; i < t * 10000; ++i)
		asm ("");
}

#endif
