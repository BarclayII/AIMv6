# Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
#
# This program is free software; you can redistribute  it and/or modify it
# under  the terms of  the GNU General  Public License as published by the
# Free Software Foundation;  either version 2 of the  License, or (at your
# option) any later version.

# Read the ARCH variable passed in the command line, find the corresponding
# architecture-specific Makefile, and load it.
include Makefile.$(ARCH)
