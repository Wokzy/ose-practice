# =============================================================================
# Build tools
NASM = nasm -f bin
PAYLOAD_SIZE = 100
GCC ?= gcc
LD ?= ld

# Variables

SRC_DIR = src
BUILD_DIR = .tmp
INCLUDE_DIR = ./include/

C_SRC = $(wildcard $(SRC_DIR)/*.c)
C_SRC_OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SRC))

GCC_FLAGS = -std=c99 -m32 -O2 -ffreestanding -no-pie -fno-pie -fno-stack-protector -I$(INCLUDE_DIR)

# =============================================================================
# Tasks

all: clean build test

.tmp/boot.o: src/boot.asm
	$(NASM) -felf src/boot.asm -o .tmp/boot.o -dKERNEL_SIZE=${PAYLOAD_SIZE}

#$(C_SRC_OBJ): $(C_SRC)
$(BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	@echo $(C_SRC)
	$(GCC) -c $(GCC_FLAGS) $< -o $@

# .tmp/kernel.o: src/kernel.c
# 	gcc -c  -I./include/ src/kernel.c -o .tmp/kernel.o

boot.img: .tmp/boot.o $(C_SRC_OBJ)
	$(LD) -m elf_i386 .tmp/boot.o $(C_SRC_OBJ) -T link.ld -o os.elf
	objcopy -I elf32-i386 -O binary os.elf boot.img
# 	dd if=/dev/zero of=boot.img bs=1024 count=1440
# 	dd if=.tmp/boot.o of=boot.img conv=notrunc
# 	dd if=zero.bin of=boot.img conv=notrunc seek=2

build: boot.img

compile: boot.img
	objdump -D -Mintel,i8086 -b binary -m i386 os.elf

clean:
	rm -f *.img
	rm -rf .tmp
	mkdir .tmp

test: build
	qemu-system-i386 -cpu pentium2 -m 1g -fda boot.img -monitor stdio -device VGA -display curses

debug: build
	qemu-system-i386 -cpu pentium2 -m 1g -fda boot.img -monitor stdio -device VGA -display curses -s -S

.PHONY: all build clean test debug
