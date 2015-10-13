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
#include <stdio.h>
#include <stdarg.h>

int snprintf(char *str, size_t size, const char *fmt, ...)
{
	int result;
	va_list ap;

	va_start(ap, fmt);
	result = vsnprintf(str, size, fmt, ap);
	va_end(ap);

	return result;
}

#define FLAG_UNSIGNED	0x001		/* unsigned integer */
#define FLAG_ZEROPAD	0x002		/* padding with zero */
#define FLAG_NEG	0x100		/* negative integer */

int vsnprintf(char *str, size_t size, const char *fmt, va_list ap)
{
#define set_ch(ch) \
	do { \
		str[pos++] = ch; \
		if (pos == size) { \
			str[size - 1] = '\0'; \
			return size; \
		} \
	} while (0)

#define is_digit(ch)	(((ch) >= '0') && ((ch) <= '9'))

	static const char *digits = "0123456789abcdef";

	long val;
	unsigned long uval;
	ssize_t width = 0;
	int base;
	int flag = 0;
	ssize_t pos = 0;
	int len;

	int buf_pos;
	char buf[25];
	char *s;

	for ( ; *fmt != '\0'; ) {
		if (*fmt == '%') {
			++fmt;
fmt_loop:		switch (*fmt) {
			case 'd':
				base = 10;
				val = va_arg(ap, long);
				if (val < 0) {
					flag |= FLAG_NEG;
					uval = -val;
				} else
					uval = val;
				goto print_uint;
			case 'o':
				base = 8;
				goto get_uint;
			case 'x':
			case 'p':
				base = 16;
				goto get_uint;
			case 'u':
				base = 10;
get_uint:			uval = va_arg(ap, unsigned long);
print_uint:			buf_pos = 0;
				while (uval > 0) {
					buf[buf_pos++] = digits[uval % base];
					uval /= base;
				}
				if (buf_pos == 0)
					buf[buf_pos++] = '0';
				if (width == 0) {
					if (flag & FLAG_NEG)
						set_ch('-');
					while ((--buf_pos) >= 0)
						set_ch(buf[buf_pos]);
				} else if ((width > 0) && (flag | FLAG_ZEROPAD)) {
					if (flag & FLAG_NEG)
						set_ch('-');
					while ((--width) >= 0) {
						if (width >= buf_pos)
							set_ch('0');
						else
							set_ch(buf[width]);
					}
				} else if (width > 0) {
					if (flag & FLAG_NEG)
						buf[buf_pos++] = '-';
					while ((--width) >= 0) {
						if (width >= buf_pos)
							set_ch(' ');
						else
							set_ch(buf[width]);
					}
				}
				++fmt;
				break;
			case 'c':
				set_ch((char)va_arg(ap, int));
				++fmt;
				break;
			case 's':
				s = va_arg(ap, char *);
				if (s == NULL)
					s = "(null)";
				for (len = 0; *s != '\0'; ++s, ++len)
					set_ch(*s);
				for (; len < width; ++len)
					set_ch(' ');
				++fmt;
				break;
			case '%':
				set_ch('%');
				++fmt;
				break;
			case '0':
				flag |= FLAG_ZEROPAD;
				++fmt;
				goto fmt_loop;
			case '1': case '2': case '3':
			case '4': case '5': case '6':
			case '7': case '8': case '9':
				for ( ; is_digit(*fmt); ++fmt) {
					width *= 10;
					width += (int)(*fmt) - (int)'0';
				}
				goto fmt_loop;
			default:
				set_ch('%');
				goto print_default;
			}
			flag = 0;
			width = 0;
			continue;
		}
print_default:	set_ch(*fmt);
		++fmt;
	}
	set_ch('\0');
	--pos;		/* exclude the null byte */
#undef FLAG_UNSIGNED
#undef FLAG_ZEROPAD
#undef FLAG_NEG
#undef is_digit
#undef set_ch
	return pos;
}

