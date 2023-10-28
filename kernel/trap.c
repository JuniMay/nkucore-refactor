#include "trap.h"
#include "printf.h"
#include "riscv.h"
#include "clock.h"
#include "sbi.h"

static int clock_ticks = 0;

void interrupt_handler(trapframe_t* trapframe) {
  uint64_t scause = read_csr(scause);

  uint64_t cause = scause & ~(1UL << 63);

  switch (cause) {
    case IRQ_S_TIMER: {
      clock_set_next_event();
      clock_ticks++;
      if (clock_ticks == 100) {
        clock_ticks = 0;
        printf("[ timer interrupt ] 100 ticks\n");
      }
      break;
    }
    default: {
      printf("unhandled interrupt: %p\n", scause);
      sbi_shutdown();
      break;
    }
  }
}

void exception_handler(trapframe_t* trapframe) {
  uint64_t scause = read_csr(scause);
  printf("unhandled exception: %d\n", scause);
}

void trap_handler(trapframe_t* trapframe) {
  uint64_t scause = read_csr(scause);
  if (scause & (1UL << 63)) {
    // interrupt
    interrupt_handler(trapframe);
  } else {
    // exception
    exception_handler(trapframe);
  }
}