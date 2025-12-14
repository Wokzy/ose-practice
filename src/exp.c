
#include "exp.h"
#include "sys.h"
#include "dtypes.h"
#include "printer.h"
#include "interrupts.h"


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
	interrupts_config config = {
		.is_trap_gate = 0,
		.auto_eoi = 0,
		.int_handler = interrupts_kernel_painc_handler
	};

	interrupts_setup_interrupts(config);
	interrupts_enable_device(TIMER);
	sti();

	sys_infinite_loop();
}


static size_t global_4 = 0;

static void print_global4_handler(interrupt_context *context) {
	printf("%d ", global_4++);
}

static void zero_global4_handler(interrupt_context *context) {
	global_4 = 0;
}

void exp_4() {
	interrupts_config config = {
		.is_trap_gate = 0,
		.auto_eoi = 1,
		.int_handler = print_global4_handler
	};

	interrupts_setup_interrupts(config);
	interrupts_enable_device(TIMER);
	sti();

	sys_infinite_loop();
}

void exp_5() {
	interrupts_config config = {
		.is_trap_gate = 0,
		.auto_eoi = 1,
		.int_handler = zero_global4_handler
	};

	interrupts_setup_interrupts(config);
	interrupts_enable_device(TIMER);
	sti();

	while (1)
		printf("%d ", global_4++);
}


static void print_global4_handler_with_eoi(interrupt_context *context) {
	printf("%d ", global_4++);
	// interrupts_send_eoi();
	// sti();
	// sys_infinite_loop();
	if (global_4 < 20) {
		// interrupts_send_eoi();
		sti();
	}
	sys_infinite_loop();
}

static void keyboard_handler(interrupt_context *context) {
	printf("%x ", sys_read_from_port(0x60));
}


void exp_8() {
	interrupts_config config = {
		.is_trap_gate = 0,
		.auto_eoi = 0,
		.int_handler = print_global4_handler_with_eoi
	};

	interrupts_setup_interrupts(config);
	interrupts_enable_device(TIMER);
	sti();

	sys_infinite_loop();
}


void exp_11() {
	interrupts_config config = {
		.is_trap_gate = 0,
		.auto_eoi = 1,
		.int_handler = print_global4_handler_with_eoi
	};

	interrupts_setup_interrupts(config);
	interrupts_enable_device(TIMER);
	sti();

	sys_infinite_loop();
}

void exp_13() {
	interrupts_config config = {
		.is_trap_gate = 0,
		.auto_eoi = 1,
		.int_handler = keyboard_handler
	};

	interrupts_setup_interrupts(config);
	interrupts_enable_device(KEYBOARD);
	sti();

	sys_infinite_loop();
}

static void wait() {
	for (size_t i = 0; i < 20; i++) {
		for (size_t j = 0; j < 50000; j++) {
			sys_write_to_port(0x80, 0x80);
		}
		printf("%d ", i);
	}
	printf("\n");
}

static void exp20_handler(interrupt_context *context) {
	// printf("hello (%x)", context->vector_index);
	if (context->vector_index == 0x20) {
		interrupts_disable_device(TIMER);
		wait();
		sti();
		wait();
	} else {
		printf("%x ", sys_read_from_port(0x60));
		sys_infinite_loop();
	}
}

void exp_20() {
	interrupts_config config = {
		.is_trap_gate = 0,
		.auto_eoi = 1,
		.int_handler = exp20_handler
	};

	interrupts_setup_interrupts(config);
	interrupts_enable_device(TIMER);
	interrupts_enable_device(KEYBOARD);
	sti();

	sys_infinite_loop();
}
