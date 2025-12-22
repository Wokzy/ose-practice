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


_Noreturn void userspace_enter_userspace(void *entry_point_ptr);
_Noreturn void userspace_exit_forwarder(uint32_t status);
