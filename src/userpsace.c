
#include "sys.h"
#include "dtypes.h"
#include "interrupts.h"
#include "memory.h"
#include "userspace.h"
#include "assert.h"

void userspace_enter_userspace(void *entry_point_ptr, void *stack_ptr) {
	userspace_enter_context context;
	context.context.cs = 0x23;
	context.context.gs = 0x1b;
	context.context.fs = 0x1b;
	context.context.es = 0x1b;
	context.context.ds = 0x1b;
	context.context.eip = (uint32_t)entry_point_ptr;
	context.context.eflags = sys_read_eflags(); // https://wiki.osdev.org/CPU_Registers_x86#EFLAGS_Register
	context.context.eflags = resetbit(context.context.eflags, 12);
	context.context.eflags = resetbit(context.context.eflags, 13);
	context.context.eflags = setbit(context.context.eflags, 9);
	context.esp = stack_ptr;
	context.ss = 0x1b;

	sys_jump_to_userspace(&context);
}
