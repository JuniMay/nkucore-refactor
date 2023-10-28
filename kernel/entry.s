
  .equ phys_virt_offset, 0xffffffff40000000

  .section .text.entry
  .globl kernel_entry
kernel_entry:
  # save harid and dtb_pa
  mv s0, a0
  mv s1, a1

  # physical address of stack top
  la sp, bootstacktop

  # set up page table
  call init_boot_pgtbl
  # set up satp
  call init_mmu

  # fix up virtual address
  li s2, phys_virt_offset

  # fix up stack pointer
  add sp, sp, s2

  # load virtual address of `kern_init`
  la a2, kern_init
  add a2, a2, s2

  # call kern_init(hartid, dtb_pa)
  mv a0, s0
  mv a1, s1
  jr a2

  .section .bss.stack
  .globl bootstack
bootstack:
  .space 4096 * 16
  .globl bootstacktop
bootstacktop:

  .section .data.boot_pgtbl
boot_pgtbl:
  .zero 512 * 8
