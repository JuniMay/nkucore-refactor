#include "mem.h"
#include "config.h"
#include "libs/panic.h"
#include "libs/printf.h"
#include "libs/riscv.h"
#include "libs/string.h"

#include "allocators/first_fit.h"
#include "swap_managers/clock.h"

/// The page array for page descriptors.
page_t* page_array;

/// The number of total pages.
size_t num_total_pages;

/// The free area used by the page allocator.
free_area_t free_area;

/// The page allocator.
page_allocator_t* page_allocator = &first_fit_page_allocator;

static void check_pmm() {
  printf("[ check_pmm ] checking pmm...\n");

  page_t *p0, *p1, *p2;

  size_t num_free_pages = page_allocator->num_free_pages();

  p0 = alloc_pages(1);
  p1 = alloc_pages(1);
  p2 = alloc_pages(1);

  assert(p0 != NULL);
  assert(p1 != NULL);
  assert(p2 != NULL);

  assert(p0->num_pages == 1);
  assert(p1->num_pages == 1);
  assert(p2->num_pages == 1);

  printf("[ check_pmm ] p0 vaddr: %p\n", p0);
  printf("[ check_pmm ] p1 vaddr: %p\n", p1);
  printf("[ check_pmm ] p2 vaddr: %p\n", p2);

  printf("[ check_pmm ] p0 paddr: %p\n", page_to_paddr(p0));
  printf("[ check_pmm ] p1 paddr: %p\n", page_to_paddr(p1));
  printf("[ check_pmm ] p2 paddr: %p\n", page_to_paddr(p2));

  free_pages(p0, 1);
  free_pages(p1, 1);
  free_pages(p2, 1);

  assert(page_allocator->num_free_pages() == num_free_pages);

  printf("[ check_pmm ] testcase 0 passed.\n");

  p0 = alloc_pages(123);
  p1 = alloc_pages(256);
  p2 = alloc_pages(179);

  assert(p0 != NULL);
  assert(p1 != NULL);
  assert(p2 != NULL);

  assert(p1->num_pages == 256);
  assert(p2->num_pages == 179);
  assert(p0->num_pages == 123);

  printf("[ check_pmm ] p0 vaddr: %p\n", p0);
  printf("[ check_pmm ] p1 vaddr: %p\n", p1);
  printf("[ check_pmm ] p2 vaddr: %p\n", p2);

  printf("[ check_pmm ] p0 paddr: %p\n", page_to_paddr(p0));
  printf("[ check_pmm ] p1 paddr: %p\n", page_to_paddr(p1));
  printf("[ check_pmm ] p2 paddr: %p\n", page_to_paddr(p2));

  free_pages(p0, 123);
  free_pages(p1, 256);
  free_pages(p2, 179);

  assert(page_allocator->num_free_pages() == num_free_pages);

  printf("[ check_pmm ] testcase 1 passed.\n");

  p0 = alloc_pages(page_allocator->num_free_pages());
  p1 = alloc_pages(17);

  assert(p0 != NULL);
  assert(p1 == NULL);
  assert(p0->num_pages == num_free_pages);
  assert(page_allocator->num_free_pages() == 0);

  printf("[ check_pmm ] p0 vaddr: %p\n", p0);
  printf("[ check_pmm ] p0 paddr: %p\n", page_to_paddr(p0));

  free_pages(p0, num_free_pages);

  assert(page_allocator->num_free_pages() == num_free_pages);

  printf("[ check_pmm ] testcase 2 passed.\n");

  printf("[ check_pmm ] all passed.\n");
}

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

  check_pmm();
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

void* kmalloc(size_t size) {
  if (size == 0) {
    return NULL;
  }

  void* ptr = NULL;
  page_t* page = NULL;

  size_t num_pages = align_up(size, PGSIZE) / PGSIZE;

  page = alloc_pages(num_pages);

  if (page == NULL) {
    return NULL;
  }

  ptr = (void*)phys_to_virt(page_to_paddr(page));

  return ptr;
}

void kfree(void* ptr, size_t size) {
  if (ptr == NULL) {
    return;
  }

  if (size == 0) {
    return;
  }

  page_t* page = paddr_to_page(virt_to_phys((uint64_t)ptr));

  size_t num_pages = align_up(size, PGSIZE) / PGSIZE;

  free_pages(page, num_pages);
}

pte_t* vmem_get_pte(uint64_t root_vaddr, uint64_t vaddr, bool create) {
  pte_t* pgtbl = (pte_t*)root_vaddr;

  for (size_t level = 2; level > 0; level--) {
    pte_t* pte = &pgtbl[PX(level, vaddr)];
    if (*pte & PTE_V) {
      pgtbl = (pte_t*)phys_to_virt(PTE2PADDR(*pte));
    } else {
      if (!create || (pgtbl = (pte_t*)kmalloc(PGSIZE)) == NULL) {
        return NULL;
      }
      memset(pgtbl, 0, PGSIZE);
      *pte = vmem_make_pte(PADDR2PPN(virt_to_phys((uint64_t)pgtbl)), PTE_V);
    }
  }

  return &pgtbl[PX(0, vaddr)];
}

/// Find the vmem_region containing the vaddr.
static vmem_region_t*
vmem_find_rigion(vmem_manager_t* manager, uint64_t vaddr) {
  assert(manager != NULL);

  // check cache first.
  if (manager->cached != NULL) {
    vmem_region_t* vmem_region =
      container_of(manager->cached, vmem_region_t, link);
    if (vmem_region->st_vaddr <= vaddr && vaddr < vmem_region->ed_vaddr) {
      return vmem_region;
    }
  }

  list_entry_t* entry = &manager->head;
  while ((entry = entry->next) != &manager->head) {
    vmem_region_t* vmem_region = container_of(entry, vmem_region_t, link);
    if (vmem_region->st_vaddr <= vaddr && vaddr < vmem_region->ed_vaddr) {
      // set cache
      manager->cached = entry;
      return vmem_region;
    }
  }

  return NULL;
}

int vmem_map_page(pte_t* pte, page_t* page, uint64_t flags) {
  assert(pte != NULL);
  assert(page != NULL);

  if (*pte & PTE_V) {
    // if the pte is valid, check the old page.
    page_t* old_page = paddr_to_page(PTE2PADDR(*pte));
    if (old_page == page) {
      // the page is already mapped.
      return 0;
    } else {
      // unmap the old page.
      page_dec_ref(old_page);
    }
  }
  page->ref += 1;
  *pte = vmem_make_pte(PADDR2PPN(page_to_paddr(page)), flags);
  return 0;
}


/// Swap in for the given virtual address.
int vmem_swap_in(vmem_manager_t* manager, uint64_t vaddr, page_t** dst_page) {
  // TODO
  return 0;
}

/// Swap out n pages.
int vmem_swap_out(vmem_manager_t* manager, uint64_t n) {
  // TODO
  return 0;
}

/// Map the page as swappable
int vmem_map_swappable(vmem_manager_t* manager, uint64_t vaddr, page_t* page, bool swap_in) {
  // TODO
  return 0;
}

int vmem_handle_pgfault(
  vmem_manager_t* manager,
  uint64_t vaddr,
  uint64_t cause
) {
  assert(manager != NULL);

  // get the corresponding vmem_region
  vmem_region_t* vmem_region = vmem_find_rigion(manager, vaddr);

  if (vmem_region == NULL) {
    printf("[ handle_pgfault ] invalid vaddr: %p\n", vaddr);
    return -1;
  }

  // offset in the page is ignored.
  vaddr = align_down(vaddr, PGSIZE);

  // get/create the pte
  pte_t* pte = vmem_get_pte(manager->pgtbl_vaddr, vaddr, true);

  if (*pte & PTE_SWAPPED) {
    // The page is swapped out before. The page should be swapped in.
    page_t* page = NULL;

    int r = vmem_swap_in(manager, vaddr, &page);

    if (r != 0) {
      panic("[ vmem_handle_pgfault ] cannot swap in.\n");
      return -1;
    }

    assert(page != NULL);

    r = vmem_map_swappable(manager, vaddr, page, true);

    if (r != 0) {
      panic("[ vmem_handle_pgfault ] cannot map swappable.\n");
      return -1;
    }

    page->vaddr = vaddr;
    assert(page->ref == 1);
  } else {
    // The page table entry is newly created. establish a new mapping.
    page_t* page = alloc_pages(1);

    if (page == NULL) {
      panic("[ vmem_handle_pgfault ] cannot allocate page.\n");
      return -1;
    }

    uint64_t flags = PTE_U | PTE_V;

    if (vmem_region->flags & VMEM_WRITE) {
      flags |= PTE_W;
    }

    if (vmem_region->flags & VMEM_READ) {
      flags |= PTE_R;
    }

    vmem_map_page(pte, page, flags);
    vmem_flush_tlb(vaddr);

    int r = vmem_map_swappable(manager, vaddr, page, false);

    if (r != 0) {
      panic("[ vmem_handle_pgfault ] cannot map swappable.\n");
      return -1;
    }

    page->vaddr = vaddr;
    assert(page->ref == 1);
  }

  return 0;
}