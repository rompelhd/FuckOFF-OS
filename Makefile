################################################################################
# FuckOFF-OS Makefile
# Build system for FuckOFF-OS x86_64 kernel
# 
# Author: Rompelhd
# License:  MIT
# Version: 0.1.0
################################################################################

#===============================================================================
# Configuration Section
#===============================================================================

# Toolchain
CC := x86_64-elf-gcc
LD := x86_64-elf-ld
ASM := nasm

# Compiler flags
CFLAGS := -I include -ffreestanding -fno-stack-protector -fno-pic -m64
ASMFLAGS := -f elf64
LDFLAGS := -n -T link.ld

# Directories
BUILD_DIR := build
DIST_DIR := dist/x86_64
ISO_DIR := iso

#===============================================================================
# File Definitions
#===============================================================================

# Source files
KERNEL_C_SOURCES := $(shell find kernel -name *.c)
KERNEL_ASM_SOURCES := $(shell find boot -name *.asm)

# Object files
KERNEL_C_OBJECTS := $(patsubst kernel/%.c, $(BUILD_DIR)/kernel/%.o, $(KERNEL_C_SOURCES))
KERNEL_ASM_OBJECTS := $(patsubst boot/%.asm, $(BUILD_DIR)/boot/%.o, $(KERNEL_ASM_SOURCES))
KERNEL_OBJECTS := $(KERNEL_C_OBJECTS) $(KERNEL_ASM_OBJECTS)

# Output files
KERNEL_BIN := $(DIST_DIR)/kernel.bin
KERNEL_ISO := $(DIST_DIR)/kernel.iso

#===============================================================================
# Build Targets
#===============================================================================

.PHONY: all
all: build-x86_64

.PHONY: build-x86_64
build-x86_64: $(KERNEL_BIN) $(KERNEL_ISO)

.PHONY: kernel
kernel: $(KERNEL_BIN)

.PHONY: iso
iso: $(KERNEL_ISO)

# Build kernel binary
$(KERNEL_BIN): $(KERNEL_OBJECTS)
        @echo "[LD] Linking kernel: $@"
        @mkdir -p $(DIST_DIR)
        $(LD) $(LDFLAGS) -o $@ $(KERNEL_OBJECTS)

# Build ISO image
$(KERNEL_ISO): $(KERNEL_BIN)
        @echo "[ISO] Preparing bootable ISO"
        $(MAKE) setup-iso
        @echo "[GRUB] Creating ISO: $@"
        grub-mkrescue -o $@ $(ISO_DIR) --compress=xz

# Compile C sources
$(BUILD_DIR)/kernel/%.o: kernel/%.c
        @echo "[CC] $<"
        @mkdir -p $(dir $@)
        $(CC) $(CFLAGS) -c $< -o $@

# Compile ASM sources
$(BUILD_DIR)/boot/%.o: boot/%.asm
        @echo "[ASM] $<"
        @mkdir -p $(dir $@)
        $(ASM) $(ASMFLAGS) $< -o $@

# Setup ISO structure
.PHONY: setup-iso
setup-iso:
        @mkdir -p $(ISO_DIR)/boot/grub
        @cp boot/grub.cfg $(ISO_DIR)/boot/grub/
        @cp $(KERNEL_BIN) $(ISO_DIR)/boot/

#===============================================================================
# Utility Targets
#===============================================================================

.PHONY: clean
clean:
        @echo "[CLEAN] Removing build artifacts"
        rm -rf $(BUILD_DIR) $(DIST_DIR) $(ISO_DIR)

.PHONY: distclean
distclean: clean
        @echo "[DISTCLEAN] Full cleanup"
        rm -f *.iso *.bin

.PHONY: run
run: $(KERNEL_ISO)
        @echo "[RUN] Starting emulator"
        qemu-system-x86_64 -cdrom $(KERNEL_ISO)

.PHONY: debug
debug: $(KERNEL_ISO)
        @echo "[DEBUG] Starting debug session"
        qemu-system-x86_64 -cdrom $(KERNEL_ISO) -s -S

.PHONY: help
help:
        @echo "FuckOFF-OS Build System"
        @echo ""
        @echo "Targets:"
        @echo "  all         - Build complete system (default)"
        @echo "  build-x86_64 - Build kernel and create ISO"
        @echo "  kernel      - Build kernel binary only"
        @echo "  iso         - Create ISO from existing kernel"
        @echo "  run         - Build and run in QEMU"
        @echo "  debug       - Build and run with debugger"
        @echo "  clean       - Remove build artifacts"
        @echo "  distclean   - Remove all generated files"
        @echo "  help        - Show this help"
        @echo ""
        @echo "Variables:"
        @echo "  CC          - C compiler: $(CC)"
        @echo "  LD          - Linker: $(LD)"
        @echo "  ASM         - Assembler: $(ASM)"

#===============================================================================
# Quality Assurance
#===============================================================================

.PHONY: check
check:
        @echo "[CHECK] Verifying toolchain"
        @which $(CC) >/dev/null || echo "ERROR: $(CC) not found"
        @which $(LD) >/dev/null || echo "ERROR: $(LD) not found"
        @which $(ASM) >/dev/null || echo "ERROR: $(ASM) not found"
        @echo "[CHECK] Toolchain verification complete"

# Print build information
.PHONY: info
info:
        @echo "Build Information:"
        @echo "  C Sources: $(words $(KERNEL_C_SOURCES)) files"
        @echo "  ASM Sources: $(words $(KERNEL_ASM_SOURCES)) files"
        @echo "  Total Objects: $(words $(KERNEL_OBJECTS)) files"
        @echo "  Build Dir: $(BUILD_DIR)"
        @echo "  Dist Dir: $(DIST_DIR)"
