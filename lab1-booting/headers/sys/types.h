/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#ifndef __ASSEMBLER__
/*
 * This macro is a wrapper for casting integer constants to unsigned longs,
 * freeing the code from compiler warnings and assembler errors (GCC
 * warns about shift count if a "UL" suffix is not appended while GAS
 * refuses to recognize the "UL" suffix).
 */
#define ULCAST(i)	(i##UL)
#else	/* __ASSEMBLER__ */
#define ULCAST(i)	i
#endif	/* !__ASSEMBLER__ */

#ifndef __ASSEMBLER__
typedef unsigned char uint8, uchar, byte, __u8, u8, uint8_t;
typedef signed char __s8, int8_t;
typedef unsigned short uint16, ushort, __u16, u16, uint16_t;
typedef signed short __s16, int16_t;
typedef unsigned int uint32, uint, __u32, u32, uint32_t;
typedef signed int __s32, int32_t;
typedef unsigned long ulong;
typedef unsigned long long uint64, __u64, u64, uint64_t;
typedef signed long long int64, __s64, int64_t;
typedef unsigned int bool;
#define false	0
#define true	1

typedef unsigned long size_t;
typedef signed long ssize_t;

typedef void *uintptr_t;

#endif

#endif

