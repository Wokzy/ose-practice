
#include "exp.h"
#include "sys.h"
#include "dtypes.h"
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


void exp_1() {
	interrupts_config config = {
		.is_trap_gate = 0,
		.auto_eoi = 0,
		.int_handler = interrupts_kernel_painc_handler
	};

	interrupts_setup_interrupts(config);
	sti();

	sys_infinite_loop();
}

void exp_2() {
	printf("from user!!!!\n");
	sys_infinite_loop();
}

uint32_t some_global = 0;

void exp_3() {
	for (;;)
		printf("from user %u\n", some_global++);
}

void exp_4() {
	uint32_t esp;
	__asm__ volatile (
		".intel_syntax noprefix\n"
		"mov %0, esp\n"
		".att_syntax\n"
		: "=r" (esp)
	);

	printf("ESP: %x\n", esp);
	sys_infinite_loop();
}

void exp_5() {
	__asm__ volatile (
		".intel_syntax noprefix\n"
		"cli\n"
		// "in al, dx\n"
		// "lgdt [esp]\n"
		// "lidt [esp]\n"
		// "ltr [esp]\n"
		// "mov cr0, eax\n"
		".att_syntax\n"
	);

	sys_infinite_loop();
}


static void exp_6() {
	for (;;)
		printf("from user %u ", some_global++);
}


static void exp_6_timer_handler(interrupt_context *context) {
	uint32_t esp;
	__asm__ volatile (
		".intel_syntax noprefix\n"
		"mov %0, esp\n"
		".att_syntax\n"
		: "=r" (esp)
	);

	printf("ESP: %x ", esp);
	sys_infinite_loop();
}


void exp_6_init() {
	interrupts_config config = {
		.is_trap_gate = 0,
		.auto_eoi = 1,
		.int_handler = exp_6_timer_handler
	};

	interrupts_setup_interrupts(config);
	interrupts_enable_device(TIMER);
	sti();

	void *user_stack = (void *)((uint32_t)malloc_undead(4096, 16) + (uint32_t)4096);
	userspace_enter_userspace(exp_6, user_stack);
}

static void exp_7() {
	sys_break_gdt();
	for (;;)
		printf("%u ", some_global++);
}

static void exp_7_timer_handler(interrupt_context *context) {
	some_global = 0;
}

void exp_7_init() {
	interrupts_config config = {
		.is_trap_gate = 0,
		.auto_eoi = 1,
		.int_handler = exp_7_timer_handler
	};

	interrupts_setup_interrupts(config);
	interrupts_enable_device(TIMER);
	sti();

	void *user_stack = (void *)((uint32_t)malloc_undead(4096, 16) + (uint32_t)4096);
	userspace_enter_userspace(exp_7, user_stack);
}

static void exp_10() {
	// sys_break_gdt();
	// assert(2 == 3);

	for (;;) {
		printf("%u ", some_global);
		__asm__ volatile (
			".intel_syntax noprefix\n"
			"int 0x80\n"
			".att_syntax\n"
		);
	}

}

static void exp_10_handler(interrupt_context *context) {
	if (context->vector_index == 0x80) {
		some_global++;
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
	allocator_init_paging();
	sti();


	void *user_stack = (void *)((uint32_t)malloc_undead(4096, 16) + (uint32_t)4096);
	userspace_enter_userspace(exp_10, user_stack);
}

