
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
	// if (some_global % 4 == 0) {
	// 	for (;;) {
	// 		__asm__ volatile (
	// 			".intel_syntax noprefix\n"
	// 			"sub esp, 4096\n"
	// 			"mov dword [esp], eax\n"
	// 			".att_syntax\n"
	// 		);
	// 	}
	// } else if (some_global % 4 == 1) {
	// 	__asm__ volatile (
	// 		".intel_syntax noprefix\n"
	// 		"mov eax, 0x42\n"
	// 		"mov dword [eax], esp\n"
	// 		".att_syntax\n"
	// 	);
	// } else if (some_global % 4 == 2) {
	// 	sys_exit(some_global);
	// } else {
	// 	__asm__ volatile (
	// 		".intel_syntax noprefix\n"
	// 		"mov eax, 0x900000\n"
	// 		"mov dword [eax], esp\n"
	// 		".att_syntax\n"
	// 	);
	// }

	sys_exit(sys_n_rec(some_global));
	// printf("hello!\n");

	// for (;;) {

	// 	__asm__ volatile (
	// 		".intel_syntax noprefix\n"
	// 		"mov ebx, 1235\n"
	// 		"int 0x80\n"
	// 		".att_syntax\n"
	// 		:
	// 		: "a" (0x10)
	// 	);
	// }

}

static void exp_10_handler(interrupt_context *context) {
	some_global++;
	if (context->vector_index == 0x80) {
		// printf("%u ", some_global);
		syscall(context);
	}
	else if (context->vector_index == 0x20) {
		// some_global = 0;
	} else if (context->vector_index == 0x0e) {
		interrupts_page_fault_handler(context);
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

	userspace_enter_userspace(exp_10);
}

