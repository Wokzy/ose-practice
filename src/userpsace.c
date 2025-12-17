
#include "sys.h"
#include "dtypes.h"
#include "interrupts.h"
#include "memory.h"
#include "userspace.h"
#include "assert.h"

void userspace_enter_userspace(void *entry_point_ptr, void *stack_ptr) {
	userspace_enter_context context;
	context.context.cs = SYS_GDT_USER_CODE;
	context.context.gs = SYS_GDT_USER_DATA;
	context.context.fs = SYS_GDT_USER_DATA;
	context.context.es = SYS_GDT_USER_DATA;
	context.context.ds = SYS_GDT_USER_DATA;
	context.context.eip = (uint32_t)entry_point_ptr;
	context.context.eflags = sys_read_eflags(); // https://wiki.osdev.org/CPU_Registers_x86#EFLAGS_Register
	context.context.eflags = resetbit(context.context.eflags, SYS_EFLAG_IOPL_0);
	context.context.eflags = resetbit(context.context.eflags, SYS_EFLAG_IOPL_1);
	context.context.eflags = setbit(context.context.eflags, SYS_EFLAG_IF);
	context.esp = stack_ptr;
	context.ss = SYS_GDT_USER_DATA;

	sys_jump_to_userspace(&context);
}
