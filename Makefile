# =============================================================================
# Build tools
NASM = nasm -f bin
PAYLOAD_SIZE = 100
GCC ?= gcc-14
LD ?= ld

# Variables

SRC_DIR = src
BUILD_DIR = .tmp
INCLUDE_DIR = ./include/

C_SRC = $(wildcard $(SRC_DIR)/*.c)
C_SRC_OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SRC))

# https://gcc.gnu.org/onlinedocs/gcc-4.3.3/gcc/i386-and-x86_002d64-Options.html#i386-and-x86_002d64-Options
GCC_FLAGS = -std=c23 -m32 -O2 -ffreestanding -no-pie -fno-pie -fno-stack-protector -I$(INCLUDE_DIR) #-msse -msse2 -msse3 -mtune=core2

# =============================================================================
# Tasks

all: build test

.tmp/boot.o: src/boot.asm
	$(NASM) -felf src/boot.asm -o .tmp/boot.o -dKERNEL_SIZE=${PAYLOAD_SIZE}

$(BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	$(GCC) -c $(GCC_FLAGS) $< -o $@

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
	qemu-system-i386 -cpu max -m 1g -fda boot.img -monitor stdio -device VGA -display curses #sdl

debug: build
	qemu-system-i386 -cpu max -m 1g -fda boot.img -monitor stdio -device VGA -display curses -s -S

.PHONY: all build clean test debug
