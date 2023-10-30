#include <stdarg.h>

#include "libs/panic.h"
#include "libs/printf.h"
#include "libs/sbi.h"
#include "libs/riscv.h"

void panic_impl(const char* file, int line, const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  printf("[ PANIC ] %s:%d: ", file, line);
  vprintf(fmt, ap);
  va_end(ap);
  disable_interrupts();

  while (1)
    ;
}