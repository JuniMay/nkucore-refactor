
.equ XLENB, 8
.equ trapframe_size, 33 * XLENB

.macro LOAD, rd, rs, offset
    ld \rd, \offset * XLENB(\rs)
.endm

.macro STORE, rs2, rs1, offset
    sd \rs2, \offset * XLENB(\rs1)
.endm

.macro STORE_LOAD_GPRS, op
    \op ra, sp, 0
    \op gp, sp, 2
    \op tp, sp, 3
    \op t0, sp, 4
    \op t1, sp, 5
    \op t2, sp, 6
    \op s0, sp, 7
    \op s1, sp, 8
    \op a0, sp, 9
    \op a1, sp, 10
    \op a2, sp, 11
    \op a3, sp, 12
    \op a4, sp, 13
    \op a5, sp, 14
    \op a6, sp, 15
    \op a7, sp, 16
    \op s2, sp, 17
    \op s3, sp, 18
    \op s4, sp, 19
    \op s5, sp, 20
    \op s6, sp, 21
    \op s7, sp, 22
    \op s8, sp, 23
    \op s9, sp, 24
    \op s10, sp, 25
    \op s11, sp, 26
    \op t3, sp, 27
    \op t4, sp, 28
    \op t5, sp, 29
    \op t6, sp, 30
.endm

.macro SAVE_REGS
    addi sp, sp, -trapframe_size
    STORE_LOAD_GPRS STORE

    csrr t0, sepc
    csrr t1, sstatus
    csrrw t2, sscratch, zero

    STORE t0, sp, 31
    STORE t1, sp, 32
    STORE t2, sp, 1
.endm

.macro RESTORE_REGS
    LOAD t0, sp, 31
    LOAD t1, sp, 32

    csrw sepc, t0
    csrw sstatus, t1

    STORE_LOAD_GPRS LOAD

    LOAD sp, sp, 1
.endm

    .section .text
    .align 2
    .globl trap_entry
trap_entry:
    csrw sscratch, sp

    SAVE_REGS

    mv a0, sp
    call trap_handler

    RESTORE_REGS

    sret
