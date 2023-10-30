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

# Use find to support multiple directory levels
KERNEL_SRC_C = $(shell find $(KERNEL_SRC_DIR) -name *.c)
KERNEL_SRC_ASM = $(shell find $(KERNEL_SRC_DIR) -name *.s)

# Modify object file targets to retain directory structure
KERNEL_OBJ_C = $(patsubst $(KERNEL_SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(KERNEL_SRC_C))
KERNEL_OBJ_ASM = $(patsubst $(KERNEL_SRC_DIR)/%.s,$(BUILD_DIR)/%.o,$(KERNEL_SRC_ASM))

KERNEL_OBJS = $(KERNEL_OBJ_C) $(KERNEL_OBJ_ASM)

KERNEL_CFLAGS = -Wall -O -g -mcmodel=medany
KERNEL_CFLAGS += -ffreestanding -fno-common -nostdlib -mno-relax -fno-stack-protector -fno-pie -no-pie
KERNEL_CFLAGS += -I$(KERNEL_INC_DIR)

KERNEL_LDSCRIPT = linker/$(PLATFORM).ld

BOOTLOADER = default

KERNEL_IMG = $(BUILD_DIR)/kernel.bin

QEMU = qemu-system-riscv64
QEMU_ARGS = -machine virt -nographic -bios $(BOOTLOADER) -kernel $(KERNEL_IMG)

.PHONY: all build clean init

all: build

# Adjust rules to account for directory structure
$(BUILD_DIR)/%.o: $(KERNEL_SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(KERNEL_SRC_DIR)/%.s
	mkdir -p $(dir $@)
	$(AS) $(KERNEL_CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel.elf: $(KERNEL_OBJS) $(KERNEL_LDSCRIPT)
	$(LD) -z max-page-size=4096 -T $(KERNEL_LDSCRIPT) $(KERNEL_OBJS) -o $@

$(KERNEL_IMG): $(BUILD_DIR)/kernel.elf
	$(OBJCOPY) --strip-all -O binary $< $@

build: $(KERNEL_IMG)

clean:
	rm -rf $(BUILD_DIR)

qemu: build
	$(QEMU) $(QEMU_ARGS)

debug: build
	$(QEMU) $(QEMU_ARGS) -S -s

gdb: build
	$(GDB) -ex "target remote localhost:1234" -ex "symbol $(BUILD_DIR)/kernel.elf"
