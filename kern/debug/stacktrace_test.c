/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <stddef.h>
#include <debug.h>
#include <stab.h>
#include <printk.h>

void call3(void)
{
	struct debug_info info;
	struct stab *last = NULL;
	printk("Inside call3\r\n");
	get_debug_info(call3, &info, &last);
	printk("%016x %s:%d %s (%08x)\r\n",
	    call3,
	    info.source_file,
	    info.line_num,
	    info.func_name,
	    info.func_addr_lo
	);
}

void call2(void)
{
	printk("Inside call2\r\n");
	call3();
}

void call1(void)
{
	printk("Inside call1\r\n");
	call2();
}

void test_stacktrace(void)
{
	printk("********** Testing stacktrace **********\r\n");
	call1();
}
