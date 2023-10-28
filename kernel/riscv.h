#ifndef KERNEL_RISCV_H_
#define KERNEL_RISCV_H_

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

#define write_csr(reg, val) asm volatile("csrw " #reg ", %0" ::"r"(val))

#define read_csr(reg)                             \
  ({                                              \
    unsigned long __tmp;                          \
    asm volatile("csrr %0, " #reg : "=r"(__tmp)); \
    __tmp;                                        \
  })

#define set_csr(reg, bit) write_csr(reg, read_csr(reg) | (bit))

#define clear_csr(reg, bit) write_csr(reg, read_csr(reg) & ~(bit))

#endif  // KERNEL_RISCV_H_