#pragma once

#include "sys.h"
#include "dtypes.h"
#include "interrupts.h"
#include "memory.h"

typedef struct {
	interrupt_context context;
	void *esp;
	alignas(4) uint16_t ss;
} userspace_enter_context;


void userspace_enter_userspace(void *entry_point_ptr, void *stack_ptr);
