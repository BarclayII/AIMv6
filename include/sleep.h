/*
 * Copyright (C) 2015 David Gao <davidgao1001@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _SLEEP_H
#define _SLEEP_H

/*
 * These functions should be provided by some clock or timer.
 * But their usage should have no difference.
 * We simply define them here. You'll get a link-time error if
 * you use one but don't have one.
 */
void sleep(int sec);
void usleep(int usec);

#endif /* _SLEEP_H */
