#ifndef KERNEL_LIBS_SBI_H_
#define KERNEL_LIBS_SBI_H_

#include "libs/types.h"

#define SBI_SET_TIMER 0
#define SBI_CONSOLE_PUTCHAR 1
#define SBI_CONSOLE_GETCHAR 2
#define SBI_CLEAR_IPI 3
#define SBI_SEND_IPI 4
#define SBI_REMOTE_FENCE_I 5
#define SBI_REMOTE_SFENCE_VMA 6
#define SBI_REMOTE_SFENCE_VMA_ASID 7
#define SBI_SHUTDOWN 8

static inline int
sbi_call(uint64_t sbi_type, uint64_t arg0, uint64_t arg1, uint64_t arg2) {
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

static inline void sbi_console_putchar(uint8_t ch) {
  sbi_call(SBI_CONSOLE_PUTCHAR, ch, 0, 0);
}

static inline int sbi_console_getchar() {
  return sbi_call(SBI_CONSOLE_GETCHAR, 0, 0, 0);
}

static inline void sbi_shutdown() {
  sbi_call(SBI_SHUTDOWN, 0, 0, 0);
}

static inline void sbi_set_timer(uint64_t stime_value) {
  sbi_call(SBI_SET_TIMER, stime_value, 0, 0);
}

#endif  // KERNEL_LIBS_SBI_H_