
#include <syscall.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>

/*
 * TODO: Add the prototypes to stdio.h
 */

int printf(const char *fmt, ...)
{
	va_list ap;
	int result;
	va_start(ap, fmt);
	result = vprintf(fmt, ap);
	va_end(ap);
	return result;
}

int vprintf(const char *fmt, va_list ap)
{
	char buf[BUFSIZ];
	int result;

	result = vsnprintf(buf, BUFSIZ, fmt, ap);
	__puts(buf);

	return result;
}
