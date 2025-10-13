
#include "dtypes.h"
#include "memory.h"
#include "printer.h"
#include "assert.h"
#include "vga.h"

void cpu_halt();

void exp() {
	while (1) {
		size_t ptr = (size_t)calloc_undead(15, 16);
		assert((ptr % 16) == 0);
		printf("0x%x\n", ptr);
	}
}

_Noreturn void kernel_entry() {
	init_printer();
	// vga_linefeed();

	// // vga_clear_screen();

	// vga_print_colored_char(0, 0, (uint8_t)'H', GREEN, BLACK, false);
	printf("hello: %x\n", 0xfafa);
	printf("hello: %d", -124);

	char *msg = calloc_undead(10, 7);
	msg[0] = 'B';
	printf("\n\n%s", msg);

	exp();

	// assert(2 == 3);

	// memset(vga, 0, 4000);
	// *((uint16_t*) 0xB8000) = 0x748;
	cpu_halt();
}
