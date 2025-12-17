
#include "vga.h"
#include "sys.h"
#include "exp.h"
#include "assert.h"
#include "dtypes.h"
#include "memory.h"
#include "printer.h"
#include "allocator.h"
#include "userspace.h"
#include "interrupts.h"



void user_function() {
	printf("from user!!!!\n");
	sys_infinite_loop();
}

_Noreturn void kernel_entry() {
	init_printer();
	// interrupts_setup_default_preset();

	// void *user_stack = (void *)((uint32_t)malloc_undead(4096, 1) + (uint32_t)4095);
	// userspace_enter_userspace(exp_7, user_stack);

	exp_10_init();

	// __asm__ volatile (
	// 	".intel_syntax noprefix\n"
	// 	"cpuid\n"
	// 	".att_syntax\n"
	// );

	// exp_20();

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
