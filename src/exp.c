
#include "exp.h"
#include "sys.h"
#include "dtypes.h"
#include "syscall.h"
#include "assert.h"
#include "printer.h"
#include "interrupts.h"
#include "userspace.h"
#include "allocator.h"


static void sti() {
	__asm__ volatile (
		".intel_syntax noprefix\n"
		"sti\n"
		".att_syntax\n"
	);
}


uint32_t some_global = 0;

static void exp_10() {
	// for (;;) {
	// 	__asm__ volatile (
	// 		".intel_syntax noprefix\n"
	// 		"push eax\n"
	// 		"push ebx\n"
	// 		"mov eax, 0x10\n"
	// 		"mov ebx, %0\n"
	// 		"int 0x80\n"
	// 		"pop eax\n"
	// 		"pop ebx\n"
	// 		".att_syntax\n"
	// 		:
	// 		: "r" (some_global)
	// 	);
	// }

	sys_exit(some_global);
}

static void exp_10_handler(interrupt_context *context) {
	if (context->vector_index == 0x80) {
		// printf("%u ", some_global);
		some_global++;
		syscall(context);
	}
	else if (context->vector_index == 0x20) {
		some_global = 0;
	} else {
		interrupts_kernel_painc_handler(context);
	}
}

void exp_10_init() {
	interrupts_config config = {
		.is_trap_gate = 0,
		.auto_eoi = 1,
		.int_handler = exp_10_handler
	};

	interrupts_setup_interrupts(config);
	interrupts_enable_device(TIMER);
	sti();

	// void *user_stack = (void *)((uint32_t)malloc_undead(4096, 16) + (uint32_t)4096);
	userspace_enter_userspace(exp_10);
}

