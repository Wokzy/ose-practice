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


_Noreturn void userspace_enter_userspace();
_Noreturn void userspace_exit_forwarder(uint32_t status);
void userspace_maybe_allocate_new_page(uint32_t cr2);

// #define USERSPACE_ENTRY_POINT (size_t)0x400000
#define USERSPACE_PACKAGE_SIZE (size_t)0x10
