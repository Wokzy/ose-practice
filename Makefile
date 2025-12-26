# =============================================================================
# Build tools
NASM = nasm -f bin
PAYLOAD_SIZE = 200
GCC ?= gcc-14
LD ?= ld

# Variables

SRC_DIR = src
BUILD_DIR = .tmp
INCLUDE_DIR = ./include/

ASM_SRC = $(wildcard $(SRC_DIR)/*.asm)
C_SRC = $(wildcard $(SRC_DIR)/*.c)
C_SRC_OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SRC))

# https://gcc.gnu.org/onlinedocs/gcc-4.3.3/gcc/i386-and-x86_002d64-Options.html#i386-and-x86_002d64-Options
GCC_FLAGS = -std=c23 -m32 -O2 -ffreestanding -no-pie -fno-pie -fno-stack-protector -Werror=int-to-pointer-cast -I$(INCLUDE_DIR) #-msse -msse2 -msse3 -mtune=core2

# =============================================================================
# Tasks

all: build test

.tmp/boot.o: src/boot.asm
	$(NASM) -felf src/boot.asm -o .tmp/boot.o -dKERNEL_SIZE=${PAYLOAD_SIZE}

.tmp/lib.o: src/lib.asm
	$(NASM) -felf src/lib.asm -o .tmp/lib.o

.tmp/std.o: src/std.asm
	$(NASM) -felf src/std.asm -o .tmp/std.o

$(BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	$(GCC) -c $(GCC_FLAGS) $< -o $@

boot.img: .tmp/boot.o .tmp/lib.o .tmp/std.o $(C_SRC_OBJ)
	$(LD) -m elf_i386 .tmp/boot.o .tmp/lib.o .tmp/std.o $(C_SRC_OBJ) -T link.ld -o os.elf
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

user_package_1: .tmp/std.o ${BUILD_DIR}/std_.o #$(wildcard $(SRC_DIR)/user_packages/*.c) $(ASM_SRC)
	$(GCC) -c $(GCC_FLAGS) $(SRC_DIR)/user_packages/app1.c -o .tmp/app1.o
	$(NASM) -felf src/startup.asm -o .tmp/startup.o
	$(LD) -m elf_i386 .tmp/std.o .tmp/startup.o .tmp/app1.o .tmp/std_.o -T link_app.ld -o app1.elf

build_with_user_packages: user_package_1 .tmp/boot.o .tmp/lib.o



.PHONY: all build clean test debug user_package_1 build_with_user_packages
