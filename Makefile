# =============================================================================
# Variables

# Build tools
NASM = nasm -f bin
PAYLOAD_SIZE = 6


# =============================================================================
# Tasks

all: clean build test

.tmp/boot.o: src/boot.asm
	$(NASM) -felf src/boot.asm -o .tmp/boot.o -dKERNEL_SIZE=${PAYLOAD_SIZE}

.tmp/kernel.o: src/kernel.c
	gcc -c -std=c99 -m32 -O2 -ffreestanding -no-pie -fno-pie -mno-sse -fno-stack-protector -I./include/ src/kernel.c -o .tmp/kernel.o

boot.img: .tmp/boot.o .tmp/kernel.o
	ld -m elf_i386 .tmp/boot.o .tmp/kernel.o -T link.ld -o os.elf
	objcopy -I elf32-i386 -O binary os.elf boot.img
# 	dd if=/dev/zero of=boot.img bs=1024 count=1440
# 	dd if=.tmp/boot.o of=boot.img conv=notrunc
# 	dd if=zero.bin of=boot.img conv=notrunc seek=2

build: boot.img

compile: boot.img
	objdump -D -Mintel,i8086 -b binary -m i386 boot.img

clean:
	rm -f *.img
	rm -rf .tmp
	mkdir .tmp

test: build
	qemu-system-i386 -cpu pentium2 -m 1g -fda boot.img -monitor stdio -device VGA -display curses

debug: build
	qemu-system-i386 -cpu pentium2 -m 1g -fda boot.img -monitor stdio -device VGA -display curses -s -S

.PHONY: all build clean test debug
