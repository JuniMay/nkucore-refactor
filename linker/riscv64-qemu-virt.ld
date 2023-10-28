OUTPUT_ARCH(riscv)
ENTRY(kernel_entry)

BASE_ADDRESS = 0xffffffffc0200000;

SECTIONS {
    . = BASE_ADDRESS;
    
    PROVIDE(_skernel = .);

    .text : ALIGN(4K) {
        *(.text.entry)
        *(.text .text.*)
    }

    .rodata : ALIGN(4K) {
        *(.rodata .rodata.*)
    }

    .data : ALIGN(4K) {
        boot_pgtbl = .;
        *(.data.boot_pgtbl)
        . = ALIGN(4K);
        *(.data .data.*)
    }

    .sdata : ALIGN(4K) {
        *(.sdata .sdata.*)
    }

    .bss : ALIGN(4K) {
        bootstack = .;
        *(.bss.stack)
        . = ALIGN(4K);
        bootstacktop = .;
        *(.bss .bss.*)
        *(.sbss .sbss.*)
    }

    PROVIDE(_ekernel = .);

	/DISCARD/ : {
        *(.comment) *(.gnu*) *(.note*) *(.eh_frame*)
    }
}