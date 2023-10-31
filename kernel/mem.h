#ifndef KERNEL_MEM_H_
#define KERNEL_MEM_H_

#include "config.h"
#include "libs/list.h"
#include "libs/riscv.h"
#include "libs/types.h"

/// Convert a virtual address to a physical address.
static inline uint64_t virt_to_phys(uint64_t vaddr) {
  return vaddr - PHYS_VIRT_OFFSET;
}

/// Convert a physical address to a virtual address.
static inline uint64_t phys_to_virt(uint64_t paddr) {
  return paddr + PHYS_VIRT_OFFSET;
}

/// Align the address down to the specified alignment.
static inline uint64_t align_down(uint64_t addr, uint64_t align) {
  return addr & ~(align - 1);
}

/// Align the address up to the specified alignment.
static inline uint64_t align_up(uint64_t addr, uint64_t align) {
  return align_down(addr + align - 1, align);
}

/// The 0 bit of flags indicates whether the page is reserved or not.
#define PG_RESERVED 0x1

/// The 1 bit of flags indicates whether the page can be allocated or not.
///
/// A page is allocable only if it is the head of a free block.
#define PG_ALLOCABLE 0x2

/// A page descriptor
typedef struct {
  /// Reference counter
  size_t ref;
  /// Flags
  uint64_t flags;
  /// Number of pages in the block
  size_t num_pages;
  /// Virtual address of the block
  uint64_t vaddr;
  /// List link
  list_entry_t link;
} page_t;

static inline void page_set_flags(page_t* page, uint64_t bits) {
  amo_set_bits(&page->flags, bits);
}

static inline void page_clear_flags(page_t* page, uint64_t bits) {
  amo_clear_bits(&page->flags, bits);
}

static inline bool page_test_flags(page_t* page, uint64_t bits) {
  return (page->flags & bits) == bits;
}

/// Free area
typedef struct {
  /// Head of the free list
  list_entry_t head;
  /// Number of free pages
  uint64_t num_free_pages;
} free_area_t;

/// Physical memory maneger
///
/// Assume the memory is flat.
typedef struct {
  /// Name of the manager/
  const char* name;

  /// Initialize the manager.
  void (*init)();

  /// Initialize the memory map.
  void (*init_memmap)(page_t*, size_t);

  /// Allocate several pages.
  page_t* (*alloc_pages)(size_t);

  /// Free several pages.
  void (*free_pages)(page_t*, size_t);

  /// Number of free pages.
  size_t (*num_free_pages)(void);
} page_allocator_t;

extern page_t* page_array;

/// Initailize the physical memory manager.
void init_pmm();

/// Allocate several pages.
page_t* alloc_pages(size_t n);

/// Free several pages.
void free_pages(page_t* base, size_t n);

static inline void page_dec_ref(page_t* page) {
  page->ref -= 1;
  if (page->ref == 0) {
    free_pages(page, page->num_pages);
  }
}

static inline page_t* paddr_to_page(uint64_t paddr) {
  return &page_array[(paddr - MEMORY_START_PADDR) >> PGSHIFT];
}

static inline uint64_t page_to_paddr(page_t* page) {
  return ((page - page_array) << PGSHIFT) + MEMORY_START_PADDR;
}

/// Process-specific virtual memory manager
typedef struct {
  /// Head of the region list
  list_entry_t head;
  /// Cached region link
  list_entry_t* cached;
  /// Virtual address of the page table
  uint64_t pgtbl_vaddr;
  /// Number of regions
  size_t num_regions;
} vmem_manager_t;

/// Kernel memory allocation
void* kmalloc(size_t size);

/// Kernel memory deallocation
void kfree(void* ptr, size_t size);

#define VMEM_READ 0x1
#define VMEM_WRITE 0x2
#define VMEM_EXEC 0x4

/// The RSW bits of PTE. According to RISC-V privileged specification, these
/// bits are used by supervisor software.
#define PTE_SWAPPED 0x100

/// Virtual memory region
typedef struct {
  /// The manager of the region
  vmem_manager_t* manager;
  /// Virtual address of the start of the region
  uint64_t st_vaddr;
  /// Virtual address of the end of the region
  uint64_t ed_vaddr;
  /// Flags
  uint64_t flags;
  /// List link
  list_entry_t link;
} vmem_region_t;

/// Create a page table entry given the ppn and flags.
static inline pte_t vmem_make_pte(uint64_t ppn, uint64_t flags) {
  return ((ppn << 10) | flags) & PTE_MASK;
}

static inline void vmem_flush_tlb(uint64_t vaddr) {
  asm volatile("sfence.vma %0" ::"r"(vaddr) : "memory");
}

/// Walk the page table and get pte.
///
/// If create is true, create the page table entry if it does not exist.
pte_t* vmem_get_pte(uint64_t root_vaddr, uint64_t vaddr, bool create);

/// Establish a mapping from the virtual address to the physical address.
///
/// This function does not flush TLB.
int vmem_map_page(pte_t* pte, page_t* page, uint64_t flags);

/// Handle page fault
int vmem_handle_pgfault(vmem_manager_t* manager, uint64_t vaddr, uint64_t cause);

/// Swap in for the given virtual address.
int vmem_swap_in(vmem_manager_t* manager, uint64_t vaddr, page_t** dst_page);

/// Swap out n pages.
int vmem_swap_out(vmem_manager_t* manager, uint64_t n);

/// Map the page as swappable
int vmem_map_swappable(vmem_manager_t* manager, uint64_t vaddr, page_t* page, bool swap_in);

#endif  // KERNEL_MEM_H_