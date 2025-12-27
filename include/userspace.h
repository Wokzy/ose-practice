#pragma once

#include "sys.h"
#include "dtypes.h"
#include "interrupts.h"
#include "memory.h"

typedef struct {
	sys_page_directory_entry *pde;
	uint32_t argc;
	sys_virtual_addr argv;
	uint32_t pid;
	interrupt_context context;
} userspace_process;

uint32_t userspace_init_process(uint32_t real_entry_point, uint32_t argc, ...);
_Noreturn void userspace_goto_userspace();
_Noreturn void userspace_exit_forwarder(uint32_t status);
void userspace_maybe_allocate_new_page(uint32_t cr2);
uint32_t userspace_get_pid();
void userspace_setup_userspace();
uint16_t userspace_in_user_space();
void userspace_timer_handler(interrupt_context *context);

// #define USERSPACE_ENTRY_POINT (size_t)0x400000
#define USERSPACE_PACKAGE_SIZE (size_t)0x10
