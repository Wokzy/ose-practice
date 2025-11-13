
#include "dtypes.h"
#include "memory.h"
#include "printer.h"
#include "allocator.h"
#include "assert.h"
#include "interrupts.h"
#include "vga.h"
#include "sys.h"
#include "exp.h"



void exp() {
	while (1) {
		size_t ptr = (size_t)calloc_undead(15, 16);
		assert((ptr % 16) == 0);
		printf("0x%x\n", ptr);
	}
}

_Noreturn void kernel_entry() {
	// init_printer();

	// __asm__ volatile (
	// 	".intel_syntax noprefix\n"
	// 	"cpuid\n"
	// 	".att_syntax\n"
	// );

	exp_20();

	// printf("HELLO!");

	// struct interrupts_config config = {0, 0, interrupts_kernel_painc_handler};
	// interrupts_setup_interrupts(config);
	// __asm__ volatile (
	// 	".intel_syntax noprefix\n"
	// 	"sti\n"
	// 	".att_syntax\n"
	// );

	// sys_zero_div();
	// sys_fake_syscall();

	// infinite_loop();
	// __asm__ volatile (
	// 	".intel_syntax noprefix\n"
	// 	"__inf_loop:\n"
	// 	"jmp __inf_loop\n"
	// 	".att_syntax\n"
	// );

	sys_infinite_loop();
	// sys_cpu_halt();
}
