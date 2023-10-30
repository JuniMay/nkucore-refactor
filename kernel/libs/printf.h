#ifndef KERNEL_LIBS_PRINTF_H_
#define KERNEL_LIBS_PRINTF_H_

#include <stdarg.h>

void vprintf(const char *fmt, va_list ap);
void printf(const char* fmt, ...);

#endif