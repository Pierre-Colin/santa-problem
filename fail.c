#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

[[noreturn]]
void
fail(const char * const restrict fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);
	vfprintf(stderr, fmt, vl);
	va_end(vl);
	thrd_exit(EXIT_FAILURE);
}
