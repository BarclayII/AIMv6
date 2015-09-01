/*
 * Copyright (C) 2015 David Gao <davidgao1001@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

/*
 * Provide integer division and modulus as Cortex-A9 does not have its own.
 * These functions should not be called directly.
 */

unsigned __aeabi_uidivmod(unsigned numerator, unsigned denominator)
{
	unsigned tmp = 1;
	unsigned shift = denominator << 1;
	unsigned quot = 0;
	/* TODO handle zero denominator */
	while (shift <= numerator) {
		denominator <<= 1;
		shift <<= 1;
		tmp <<= 1;
	}
	while (tmp > 0) {
		if (numerator > denominator){
			quot += tmp;
			numerator -= denominator;
		}
		denominator >>= 1;
		tmp >>= 1;
	}
	asm volatile (
		"mov r1, %[rem]"::[rem] "r" (numerator)
	);
	return quot;
}
