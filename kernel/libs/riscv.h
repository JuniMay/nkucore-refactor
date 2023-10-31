#ifndef KERNEL_LIBS_RISCV_H_
#define KERNEL_LIBS_RISCV_H_

#include "libs/types.h"

typedef uint64_t pte_t;

#define IRQ_U_SOFT 0
#define IRQ_S_SOFT 1
#define IRQ_H_SOFT 2
#define IRQ_M_SOFT 3
#define IRQ_U_TIMER 4
#define IRQ_S_TIMER 5
#define IRQ_H_TIMER 6
#define IRQ_M_TIMER 7
#define IRQ_U_EXT 8
#define IRQ_S_EXT 9
#define IRQ_H_EXT 10
#define IRQ_M_EXT 11
#define IRQ_COP 12
#define IRQ_HOST 13

#define CAUSE_MISALIGNED_FETCH 0x0
#define CAUSE_FAULT_FETCH 0x1
#define CAUSE_ILLEGAL_INSTRUCTION 0x2
#define CAUSE_BREAKPOINT 0x3
#define CAUSE_MISALIGNED_LOAD 0x4
#define CAUSE_FAULT_LOAD 0x5
#define CAUSE_MISALIGNED_STORE 0x6
#define CAUSE_FAULT_STORE 0x7
#define CAUSE_USER_ECALL 0x8
#define CAUSE_SUPERVISOR_ECALL 0x9
#define CAUSE_HYPERVISOR_ECALL 0xa
#define CAUSE_MACHINE_ECALL 0xb

#define IRQ_S_TIMER 5

#define MIP_STIP (1 << IRQ_S_TIMER)

#define SSTATUS_SIE 0x00000002

#define PGSIZE 4096
#define PGSHIFT 12

#define PTE_V 0x001
#define PTE_R 0x002
#define PTE_W 0x004
#define PTE_X 0x008
#define PTE_U 0x010
#define PTE_G 0x020
#define PTE_A 0x040
#define PTE_D 0x080

/// Get the page index of the specified virtual address at the specified level.
#define PX(level, vaddr) \
  (((uint64_t)(vaddr) >> (PGSHIFT + (9 * (level)))) & 0x1ff)

/// Page table entry mask.
///
/// Accroding to RISC-V privileged specification, highest bits are reserved for
/// future use and must be zero. Note that bit 63 is used by Svnapot extension
/// and bits 62-61 are used by Svpbmt extension.
#define PTE_MASK 0x003ffffffffffff

/// Mask to extract the physical address from a pte.
#define PTE_PPN_MASK 0x003ffffffffffc00

/// Convert the pte to a physical address.
#define PTE2PADDR(pte) (((uint64_t)(pte) & PTE_PPN_MASK) << 2)

/// Convert the physical address to a pte.
#define PADDR2PPN(paddr) ((((uint64_t)(paddr) >> 2) & PTE_PPN_MASK) >> 10)

#define write_csr(reg, val) asm volatile("csrw " #reg ", %0" ::"r"(val))

#define read_csr(reg)                             \
  ({                                              \
    unsigned long __tmp;                          \
    asm volatile("csrr %0, " #reg : "=r"(__tmp)); \
    __tmp;                                        \
  })

#define set_csr(reg, bit) write_csr(reg, read_csr(reg) | (bit))

#define clear_csr(reg, bit) write_csr(reg, read_csr(reg) & ~(bit))

#define read_gpr(reg)                           \
  ({                                            \
    unsigned long __tmp;                        \
    asm volatile("mv %0, " #reg : "=r"(__tmp)); \
    __tmp;                                      \
  })

static inline void enable_interrupts() {
  set_csr(sstatus, SSTATUS_SIE);
}

static inline void disable_interrupts() {
  clear_csr(sstatus, SSTATUS_SIE);
}

static inline bool interrupts_enabled() {
  return read_csr(sstatus) & SSTATUS_SIE;
}

static inline bool save_interrupts() {
  bool enabled = interrupts_enabled();
  disable_interrupts();
  return enabled;
}

static inline void restore_interrupts(bool enabled) {
  if (enabled) {
    enable_interrupts();
  }
}

static inline void amo_set_bits(volatile void* ptr, uint64_t bits) {
  asm volatile("amoor.d zero, %1, %0"
               : "+A"(*(volatile uint64_t*)ptr)
               : "r"(bits));
}

static inline void amo_clear_bits(volatile void* ptr, uint64_t bits) {
  asm volatile("amoand.d zero, %1, %0"
               : "+A"(*(volatile uint64_t*)ptr)
               : "r"(~bits));
}

#endif  // KERNEL_LIBS_RISCV_H_