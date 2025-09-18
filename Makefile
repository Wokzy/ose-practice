# =============================================================================
# Variables

# Build tools
NASM = nasm -f bin 
PAYLOAD_SIZE = 512


# =============================================================================
# Tasks

all: clean build test

.tmp/boot.bin: src/boot.asm
	$(NASM) src/boot.asm -o .tmp/boot.bin

boot.img: .tmp/boot.bin
	python3 generate_bytes.py --size ${PAYLOAD_SIZE} --output_fname foo.bin
	dd if=/dev/zero of=boot.img bs=1024 count=1440
	dd if=.tmp/boot.bin of=boot.img conv=notrunc
	dd if=foo.bin of=boot.img conv=notrunc seek=1

build: boot.img

compile: .tmp/boot.bin
	objdump -D -Mintel,i8086 -b binary -m i386 .tmp/boot.bin

clean:
	rm -f *.img
	rm -rf .tmp
	mkdir .tmp

test: build
	qemu-system-i386 -cpu pentium2 -m 1g -fda boot.img -monitor stdio -device VGA -display curses

debug: build
	qemu-system-i386 -cpu pentium2 -m 1g -fda boot.img -monitor stdio -device VGA -display curses -s -S

.PHONY: all build clean test debug
