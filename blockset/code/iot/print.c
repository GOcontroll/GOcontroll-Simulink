#include <stdarg.h>
#include <stdio.h>

#include "SEGGER_RTT.h"

void dbg(char* format, ...) {
#if DEBUG == 1
	va_list args;
	va_start(args, format);
	SEGGER_RTT_vprintf(0, format, &args);
	va_end(args);
#endif
}
void info(char* format, ...) {
#if DEBUG == 1
	va_list args;
	va_start(args, format);
	SEGGER_RTT_vprintf(0, format, &args);
	va_end(args);
#endif
}
void err(char* format, ...) {
#if DEBUG == 1
	va_list args;
	va_start(args, format);
	SEGGER_RTT_vprintf(0, format, &args);
	va_end(args);
#endif
}
