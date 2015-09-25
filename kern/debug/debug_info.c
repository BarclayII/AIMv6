/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <sys/types.h>
#include <stddef.h>
#include <debug.h>
#include <stab.h>
#include <util.h>

/*
 * Value field of stabs representing address is not always aligned to 4
 * bytes on at least MIPS.  Not sure whether it's true on ARM.
 * asm/util.h is for fixing the address alignment of instructions.
 *
 * Please provide feedback.
 */
#include <asm/util.h>

extern struct stab __STAB_BEGIN__[], __STAB_END__[];
extern char __STABSTR_BEGIN__[], __STABSTR_END__[];

/*
 * Find the source file stab by checking the type field.
 * All source file stabs with type N_SO and desc field 0 should be ignored
 * as they either contain null string or holds the file name of temporary
 * overall assembly.
 */
static inline bool is_source(struct stab *stab)
{
	return ((stab->n_type == N_SO && stab->n_desc != 0) ||
	    (stab->n_type == N_SOL));
}

static inline bool is_textline(struct stab *stab)
{
	return (stab->n_type == N_SLINE);
}

static inline bool is_func(struct stab *stab)
{
	return (stab->n_type = N_FUN);
}

/*
 * Get debug info of an instruction
 *
 * Note that GCC is an optimizing compiler: GCC may arrange, add, and/or
 * remove instructions for performance, thus messing the instruction
 * structure of function.  For example, there may be multiple
 * rets in one function in i386 disassembly.
 *
 * Perhaps the best solution is to perform linear search in the stabs
 * section, as stabs are actually not ordered by address.
 *
 * Parameters:
 * @addr	: Instruction address
 * @info	: Debug info structure
 * @last	: Last stab with the same instruction address
 *
 * Return value:
 * >0 if found.
 * 0 if no stab left.
 * <0 if didn't find any.
 */
int get_debug_info(uintptr_t addr, struct debug_info *info, struct stab **last)
{
	struct stab *cur_stab;
	char *source_filename = NULL, *func_name = NULL;
	unsigned int source_addr32 = 0, func_addr32 = 0, line_addr32 = 0;
	unsigned int addr32 = lo32((unsigned long)addr);
	bool found = false;

	addr32 = FIX_INSTRUCTION_ADDR(addr32);

	for (cur_stab = __STAB_BEGIN__; cur_stab != __STAB_END__; ++cur_stab) {
		if (is_source(cur_stab)) {
			/* Perform potential fixes for address */
			source_addr32 = FIX_INSTRUCTION_ADDR(cur_stab->n_value);
			source_filename = &__STABSTR_BEGIN__[cur_stab->n_strx];
		} else if (is_textline(cur_stab)) {
			line_addr32 = source_addr32 + cur_stab->n_value;
			if (line_addr32 == addr32) {
				found = true;
				if (*last < cur_stab) {
					info->source_file = source_filename;
					info->func_name = func_name;
					info->func_addr_lo = func_addr32;
					info->line_num = cur_stab->n_desc;
					*last = cur_stab;
					return 1;
				}
			} else if (found) {
				return 0;
			}
		} else if (is_func(cur_stab)) {
			func_addr32 = cur_stab->n_value;
			func_name = &__STABSTR_BEGIN__[cur_stab->n_strx];
		}
	}
	return -1;
}
