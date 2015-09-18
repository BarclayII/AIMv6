/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _DEBUG_H
#define _DEBUG_H

#include <stab.h>

struct debug_info {
	char		*source_file;
	char		*func_name;
	unsigned int	func_addr_lo;
	unsigned int	line_num;
};

int get_debug_info(uintptr_t addr, struct debug_info *info, struct stab **last);

#endif
