#include "sbi.h"

int sbi_call(uint64_t sbi_type, uint64_t arg0, uint64_t arg1, uint64_t arg2) {
  register uint64_t a0 asm("a0") = arg0;
  register uint64_t a1 asm("a1") = arg1;
  register uint64_t a2 asm("a2") = arg2;
  register uint64_t a7 asm("a7") = sbi_type;
  asm volatile("ecall"
               : "=r"(a0)
               : "r"(a0), "r"(a1), "r"(a2), "r"(a7)
               : "memory");
  return a0;
}

void sbi_console_putchar(uint8_t ch) {
  sbi_call(SBI_CONSOLE_PUTCHAR, ch, 0, 0);
}

int sbi_console_getchar() {
  return sbi_call(SBI_CONSOLE_GETCHAR, 0, 0, 0);
}

void sbi_shutdown() {
  sbi_call(SBI_SHUTDOWN, 0, 0, 0);
}

void sbi_set_timer(uint64_t stime_value) {
  sbi_call(SBI_SET_TIMER, stime_value, 0, 0);
}