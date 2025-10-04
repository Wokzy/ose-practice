
#include "dtypes.h"
#include "memory.h"
#include "vga.h"

void cpu_halt();

_Noreturn void kernel_entry() {
	vga_linefeed();
	vga_linefeed();

	// vga_clear_screen();

	vga_print_colored_char(0, 0, (uint8_t)'H', GREEN, BLACK, false);

	// memset(vga, 0, 4000);
	// *((uint16_t*) 0xB8000) = 0x748;
	cpu_halt();
}
