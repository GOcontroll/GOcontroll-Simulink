#include <stdarg.h>
#include <stdio.h>
void dbg(char* format, ...) {
#if DEBUG == 1
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
#endif
}
void info(char* format, ...) {
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}
void err(char* format, ...) {
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
}
