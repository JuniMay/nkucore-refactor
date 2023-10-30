#include "mem.h"
#include "config.h"
#include "printf.h"
#include "riscv.h"

page_t* page_array;
size_t num_total_pages;

void init_pmm() {

  extern char _skernel[];
  extern char _ekernel[];

  uint64_t kernel_st_vaddr = (uint64_t)_skernel;
  uint64_t kernel_ed_vaddr = (uint64_t)_ekernel;

  uint64_t kernel_st_paddr = virt_to_phys(kernel_st_vaddr);
  uint64_t kernel_ed_paddr = virt_to_phys(kernel_ed_vaddr);

  uint64_t memory_ed_paddr = MEMORY_END_PADDR;

  num_total_pages = memory_ed_paddr / PGSIZE;

  // TODO
}