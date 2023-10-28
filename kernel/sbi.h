#ifndef KERNEL_SBI_H_
#define KERNEL_SBI_H_

#include "types.h"

#define SBI_SET_TIMER 0
#define SBI_CONSOLE_PUTCHAR 1
#define SBI_CONSOLE_GETCHAR 2
#define SBI_CLEAR_IPI 3
#define SBI_SEND_IPI 4
#define SBI_REMOTE_FENCE_I 5
#define SBI_REMOTE_SFENCE_VMA 6
#define SBI_REMOTE_SFENCE_VMA_ASID 7
#define SBI_SHUTDOWN 8

int sbi_call(uint64_t sbi_type, uint64_t arg0, uint64_t arg1, uint64_t arg2);

void sbi_console_putchar(uint8_t ch);
int sbi_console_getchar();

void sbi_shutdown();
void sbi_set_timer(uint64_t stime_value);

#endif