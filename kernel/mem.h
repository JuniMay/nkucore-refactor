#ifndef KERNEL_MEM_H_
#define KERNEL_MEM_H_

#include "list.h"
#include "types.h"
#include "config.h"

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
#define PG_RESERVED 0x0

/// The 1 bit of flags indicates whether the page can be allocated or not.
/// 
/// A page is allocable only if it is the head of a free block.
#define PG_ALLOCABLE 0x1

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
  const char *name;

  /// Initialize the manager.
  void (*init)();

  /// Initialize the memory map.
  void (*init_memmap)(page_t*, uint64_t);

  /// Allocate several pages.
  page_t* (*alloc_pages)(uint64_t);

  /// Free several pages.
  void (*free_pages)(page_t*, uint64_t);

  /// Number of free pages.
  size_t (*num_free_pages)(void);
} pmm_t;

extern page_t* page_array;
extern size_t num_total_pages;

extern pmm_t *pmm;

/// Initailize the physical memory manager.
void init_pmm();

/// Allocate several pages.
page_t* alloc_pages(size_t num_pages);

/// Free several pages.
void free_pages(page_t* base, size_t num_pages);

#endif