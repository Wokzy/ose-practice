
#include "dtypes.h"
#include "memory.h"
#include "printer.h"
#include "allocator.h"
#include "assert.h"
#include "interrupts.h"
#include "vga.h"

void cpu_halt();
void fake_syscall();
void infinite_loop();
void zero_div();

void exp() {
	while (1) {
		size_t ptr = (size_t)calloc_undead(15, 16);
		assert((ptr % 16) == 0);
		printf("0x%x\n", ptr);
	}
}

_Noreturn void kernel_entry() {
	init_printer();

	setup_interrupts();

	// zero_div();
	fake_syscall();

	infinite_loop();
}
