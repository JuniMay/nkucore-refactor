#ifndef KERNEL_TRAP_H_
#define KERNEL_TRAP_H_

#include "libs/types.h"

typedef struct {
    uint64_t gprs[31];
    uint64_t sepc;
    uint64_t sstatus;
} trapframe_t;

void trap_handler(trapframe_t* trapframe);

void interrupt_handler(trapframe_t* trapframe);
void exception_handler(trapframe_t* trapframe);

#endif  // KERNEL_TRAP_H_