#include "mem.h"
#include "config.h"
#include "libs/printf.h"
#include "libs/riscv.h"

#include "allocators/first_fit.h"

page_t* page_array;
size_t num_total_pages;
free_area_t free_area;

page_allocator_t* page_allocator = &first_fit_page_allocator;

void init_pmm() {
  extern char _skernel[];
  extern char _ekernel[];

  uint64_t kernel_st_vaddr = (uint64_t)_skernel;
  uint64_t kernel_ed_vaddr = (uint64_t)_ekernel;

  uint64_t kernel_st_paddr = virt_to_phys(kernel_st_vaddr);
  uint64_t kernel_ed_paddr = virt_to_phys(kernel_ed_vaddr);

  uint64_t memory_st_paddr = MEMORY_START_PADDR;
  uint64_t memory_ed_paddr = MEMORY_END_PADDR;

  num_total_pages = (memory_ed_paddr - memory_st_paddr) / PGSIZE;

  page_array = (page_t*)align_up(kernel_ed_vaddr, PGSIZE);

  for (size_t i = 0; i < num_total_pages; i++) {
    page_set_flags(page_array + i, PG_RESERVED);
  }

  uint64_t allocable_mem_st_vaddr =
    align_up((uint64_t)page_array + num_total_pages * sizeof(page_t), PGSIZE);

  uint64_t allocable_mem_st_paddr = virt_to_phys(allocable_mem_st_vaddr);
  uint64_t allocable_mem_ed_paddr = align_down(memory_ed_paddr, PGSIZE);

  // clang-format off
  printf("[ init_pmm ] memory(phys):     %p - %p\n", memory_st_paddr, memory_ed_paddr);
  printf("[ init_pmm ] kernel(phys):     %p - %p\n", kernel_st_paddr, kernel_ed_paddr);
  printf("[ init_pmm ] kernel(virt):     %p - %p\n", kernel_st_vaddr, kernel_ed_vaddr);
  printf("[ init_pmm ] page_array(virt): %p - %p\n", page_array, page_array + num_total_pages);
  printf("[ init_pmm ] total pages:      %d\n", num_total_pages);
  printf("[ init_pmm ] allocable(phys):  %p - %p\n", allocable_mem_st_paddr, allocable_mem_ed_paddr);
  printf("[ init_pmm ] allocable pages:  %d\n", (allocable_mem_ed_paddr - allocable_mem_st_paddr) / PGSIZE);
  // clang-format on

  page_allocator->init();
  page_allocator->init_memmap(
    paddr_to_page(allocable_mem_st_paddr),
    (allocable_mem_ed_paddr - allocable_mem_st_paddr) / PGSIZE
  );
}

page_t* alloc_pages(size_t n) {
  bool interrupts_enabled = save_interrupts();
  page_t* page = page_allocator->alloc_pages(n);
  restore_interrupts(interrupts_enabled);
  return page;
}

void free_pages(page_t* base, size_t n) {
  bool interrupts_enabled = save_interrupts();
  page_allocator->free_pages(base, n);
  restore_interrupts(interrupts_enabled);
}
