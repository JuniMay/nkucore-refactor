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

void kern_init(uint64_t hartid, uint64_t dtb_pa) {
  printf(banner);

  printf("hartid: %d\n", hartid);
  printf("dtb_pa: %p\n", dtb_pa);
  printf("kernel: %p - %p\n", _skernel, _ekernel);
  printf("trap_entry: %p\n", trap_entry);
  printf("kern_init: %p\n", kern_init);
  printf("boot_pgtbl: %p\n", boot_pgtbl);

  // set stvec
  write_csr(stvec, (uint64_t)trap_entry);

  clock_init();

  // enable irq
  set_csr(sstatus, SSTATUS_SIE);

  while (1)
    ;
}