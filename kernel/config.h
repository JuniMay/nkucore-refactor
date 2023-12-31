#ifndef KERNEL_CONFIG_H_
#define KERNEL_CONFIG_H_

#define MEMORY_START_PADDR 0x80000000

/// Physical address of the end of the usable memory.
#define MEMORY_END_PADDR 0x88000000

/// Assume there is a linear mapping from physical address to virtual address.
#define PHYS_VIRT_OFFSET 0xffffffff40000000

#endif  // KERNEL_CONFIG_H_