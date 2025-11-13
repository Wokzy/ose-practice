
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

	struct interrupts_config config = {0, 0, interrupts_kernel_painc_handler};
	interrupts_setup_interrupts(config);
	__asm__ volatile (
		".intel_syntax noprefix\n"
		"sti\n"
		".att_syntax\n"
	);

	// zero_div();
	fake_syscall();

	// infinite_loop();
	// __asm__ volatile (
	// 	".intel_syntax noprefix\n"
	// 	"__inf_loop:\n"
	// 	"jmp __inf_loop\n"
	// 	".att_syntax\n"
	// );

	infinite_loop();
}
