
TOOLCHAIN_PREFIX = riscv64-unknown-elf-

PLATFORM = riscv64-qemu-virt

CC = $(TOOLCHAIN_PREFIX)gcc
AS = $(TOOLCHAIN_PREFIX)gcc
LD = $(TOOLCHAIN_PREFIX)ld

OBJCOPY = $(TOOLCHAIN_PREFIX)objcopy
OBJDUMP = $(TOOLCHAIN_PREFIX)objdump

BUILD_DIR = build

GDB = gdb-multiarch

KERNEL_INC_DIR = kernel
KERNEL_SRC_DIR = kernel

# kernel source directory is flatten, just wildcard all .c/.s files
KERNEL_SRC_C = $(wildcard $(KERNEL_SRC_DIR)/*.c)
KERNEL_SRC_ASM = $(wildcard $(KERNEL_SRC_DIR)/*.s)

KERNEL_OBJ_C = $(addprefix $(BUILD_DIR)/, $(notdir $(KERNEL_SRC_C:.c=.o)))
KERNEL_OBJ_ASM += $(addprefix $(BUILD_DIR)/, $(notdir $(KERNEL_SRC_ASM:.s=.o)))

KERNEL_OBJS = $(KERNEL_OBJ_C) $(KERNEL_OBJ_ASM)

KERNEL_CFLAGS = -Wall -Werror -O -g -mcmodel=medany
KERNEL_CFLAGS += -ffreestanding -fno-common -nostdlib -mno-relax -fno-stack-protector -fno-pie -no-pie
KERNEL_CFLAGS += -I$(KERNEL_INC_DIR)

KERNEL_LDSCRIPT = linker/$(PLATFORM).ld

BOOTLOADER = default

KERNEL_IMG = $(BUILD_DIR)/kernel.bin

QEMU = qemu-system-riscv64
QEMU_ARGS = -machine virt -nographic -bios $(BOOTLOADER) -kernel $(KERNEL_IMG)

.PHONY: all build clean init

all: build

$(KERNEL_OBJ_C): $(BUILD_DIR)/%.o: $(KERNEL_SRC_DIR)/%.c
	$(CC) $(KERNEL_CFLAGS) -c $< -o $@

$(KERNEL_OBJ_ASM): $(BUILD_DIR)/%.o: $(KERNEL_SRC_DIR)/%.s
	$(AS) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel.elf: $(KERNEL_OBJS) $(KERNEL_LDSCRIPT)
	$(LD) -z max-page-size=4096 -T $(KERNEL_LDSCRIPT) $(KERNEL_OBJS) -o $@

$(KERNEL_IMG): $(BUILD_DIR)/kernel.elf
	$(OBJCOPY) --strip-all -O binary $< $@

build: init $(KERNEL_IMG)

clean:
	rm -rf $(BUILD_DIR)

qemu: build
	$(QEMU) $(QEMU_ARGS)

init:
	mkdir -p $(BUILD_DIR)