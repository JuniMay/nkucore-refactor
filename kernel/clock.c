#include "clock.h"
#include "printf.h"
#include "riscv.h"
#include "sbi.h"

static uint64_t timebase = 0;

static inline uint64_t get_cycles(void) {
  uint64_t n;
  asm volatile("rdtime %0" : "=r"(n));
  return n;
}

void clock_init() {
  timebase = 1e7 / 100;
  clock_set_next_event();
  set_csr(sie, MIP_STIP);

  printf("sie: %p\n", read_csr(sie));

  printf("[ clock init ] set up timer interrupts\n");
}

void clock_set_next_event() {
  sbi_set_timer(get_cycles() + timebase);
}