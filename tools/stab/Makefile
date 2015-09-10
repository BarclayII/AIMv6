# Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
#
# This program is free software; you can redistribute  it and/or modify it
# under  the terms of  the GNU General  Public License as published by the
# Free Software Foundation;  either version 2 of the  License, or (at your
# option) any later version.

# readstab runs on host, so one really shouldn't use the cross-compiling
# gcc.
CC		= gcc
CFLAGS		= -I../..

BINARY		= readstab

SRCS		= readstab.c

all: $(BINARY)

$(BINARY): $(SRCS)
	$(CC) $(CFLAGS) -o $(BINARY) $(SRCS)

clean:
	rm -f $(BINARY)

.PHONY: all clean
