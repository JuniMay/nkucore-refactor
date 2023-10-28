
    .section .text.entry
    .globl kernel_entry
kernel_entry:
    la sp, bootstacktop
    tail kern_init

    .section .bss.stack
    .globl bootstack
bootstack:
    .space 4096 * 16
    .globl bootstacktop
bootstacktop:

    .section .data.boot_pgtbl
boot_pgtbl:
    .zero 512 * 8