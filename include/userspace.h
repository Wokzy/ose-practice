#pragma once

#include "sys.h"
#include "dtypes.h"
#include "interrupts.h"
#include "memory.h"

typedef struct {
	interrupt_context context;
	uint32_t esp;
	alignas(4) uint16_t ss;
} userspace_enter_context;


typedef struct {
	sys_page_directory_entry *pde;
	uint32_t real_entry_point;
	uint32_t argc;
	sys_virtual_addr argv;
} userspace_process;

userspace_process userspace_init_process(uint32_t real_entry_point, uint32_t argc, ...);
_Noreturn void userspace_start_process(userspace_process process);
_Noreturn void userspace_exit_forwarder(uint32_t status);
void userspace_maybe_allocate_new_page(uint32_t cr2);

// #define USERSPACE_ENTRY_POINT (size_t)0x400000
#define USERSPACE_PACKAGE_SIZE (size_t)0x10
