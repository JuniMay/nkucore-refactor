#include "trap.h"
#include "clock.h"
#include "libs/printf.h"
#include "libs/riscv.h"
#include "libs/sbi.h"
#include "libs/panic.h"

static int clock_ticks = 0;

void interrupt_handler(trapframe_t* trapframe, uint64_t scause) {
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
      panic("unhandled interrupt: %p\n", scause);
      break;
    }
  }
}

void exception_handler(trapframe_t* trapframe, uint64_t scause) {
  panic("unhandled exception: %p\n", scause);
}

void trap_handler(trapframe_t* trapframe) {
  uint64_t scause = read_csr(scause);
  if (scause & (1UL << 63)) {
    // interrupt
    interrupt_handler(trapframe, scause);
  } else {
    // exception
    exception_handler(trapframe, scause);
  }
}