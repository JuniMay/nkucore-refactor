#include "clock.h"
#include "printf.h"
#include "riscv.h"
#include "sbi.h"
#include "trap.h"
#include "types.h"

const char banner[] = 
"\n"
"##\\   ##\\ ##\\   ##\\ ##\\   ##\\  ######\\   ######\\  #######\\  ########\\ \n"
"###\\  ## |## | ##  |## |  ## |##  __##\\ ##  __##\\ ##  __##\\ ##  _____|\n"
"####\\ ## |## |##  / ## |  ## |## /  \\__|## /  ## |## |  ## |## |      \n"
"## ##\\## |#####  /  ## |  ## |## |      ## |  ## |#######  |#####\\    \n"
"## \\#### |##  ##<   ## |  ## |## |      ## |  ## |##  __##< ##  __|   \n"
"## |\\### |## |\\##\\  ## |  ## |## |  ##\\ ## |  ## |## |  ## |## |      \n"
"## | \\## |## | \\##\\ \\######  |\\######  | ######  |## |  ## |########\\ \n"
"\\__|  \\__|\\__|  \\__| \\______/  \\______/  \\______/ \\__|  \\__|\\________|\n"
"\n";


extern char _skernel[];
extern char _ekernel[];

extern char trap_entry[];
extern uint64_t boot_pgtbl[];

void init_boot_pgtbl() {
  boot_pgtbl[2] = (0x80000 << 10) | 0xef;
  boot_pgtbl[511] = (0x80000 << 10) | 0xef;
}

void init_mmu() {
  write_csr(satp, ((uint64_t)boot_pgtbl >> 12) | (8UL << 60));
  asm volatile("sfence.vma");
}

void kern_init(uint64_t hartid, uint64_t dtb_pa) {
  printf(banner);

  printf("hartid:     %d\n", hartid);
  printf("dtb_pa:     %p\n", dtb_pa);
  printf("kernel:     %p - %p\n", _skernel, _ekernel);
  printf("trap_entry: %p\n", trap_entry);
  printf("kern_init:  %p\n", kern_init);
  printf("boot_pgtbl: %p\n", boot_pgtbl);
  printf("sp:         %p\n", read_gpr(sp));

  // set stvec
  write_csr(stvec, (uint64_t)trap_entry);

  clock_init();

  // enable irq
  set_csr(sstatus, SSTATUS_SIE);

  while (1)
    ;
}