
#include <stdarg.h>

#include "sys.h"
#include "dtypes.h"
#include "interrupts.h"
#include "memory.h"
#include "userspace.h"
#include "allocator.h"
#include "printer.h"
#include "panic.h"
#include "assert.h"
#include "std.h"


/*
- paging выключен заранее
- выделяем программе таблицу директорий, таблицу страниц для 0x0-0x400000 и 1 страницу для стека
- включаем paging на выделенной таблице директорий

на exit:
- выключам paging
- чистим таблицу директорий
- ** делаем, что хотим =) ** (можно и программу запустить заного)
*/


static sys_page_directory_entry *get_cr3() {
	sys_page_directory_entry *cr3;

	__asm__ volatile (
		".intel_syntax noprefix\n"
		"mov %0, cr3\n"
		".att_syntax\n"
		: "=r" (cr3)
	);

	return cr3;
}


static sys_virtual_addr allocate_argv(sys_page_directory_entry *pde, uint32_t argc, va_list argv) {

	if (argc == 0) {
		return (sys_virtual_addr){0, 0, 0};
	}

	sys_virtual_addr *vir_argv = allocator_alloc_page();
	sys_page_table_entry *arg_pte = allocator_calloc_page();

	pde[SYS_PD_SIZE - 3].page_table_addr = ((uint32_t)arg_pte) >> 12;
	pde[SYS_PD_SIZE - 3].us = 1;
	pde[SYS_PD_SIZE - 3].rw = 1;
	pde[SYS_PD_SIZE - 3].enabled = 1;

	arg_pte[0].frame_addr = ((uint32_t)vir_argv) >> 12;
	arg_pte[0].us = 1;
	arg_pte[0].rw = 1;
	arg_pte[0].enabled = 1;

	for (uint32_t i = 0; i < argc; i++) {
		char *arg_ptr = allocator_calloc_page();
		char *real_arg = va_arg(argv, char *);
		memcpy(arg_ptr, real_arg, strlen(real_arg));

		arg_pte[i + 1].frame_addr = ((uint32_t)arg_ptr) >> 12;
		arg_pte[i + 1].us = 1;
		arg_pte[i + 1].rw = 1;
		arg_pte[i + 1].enabled = 1;


		vir_argv[i].page_directory_index = SYS_PD_SIZE - 3;
		vir_argv[i].page_table_index = i + 1;
		vir_argv[i].offset = 0;
	}

	sys_virtual_addr res;
	res.page_directory_index = SYS_PD_SIZE - 3;
	res.page_table_index = 0;
	res.offset = 0;

	return res;
}


userspace_process userspace_init_process(uint32_t real_entry_point, uint32_t argc, ...) {
	assert(sizeof(sys_virtual_addr) == sizeof(void *));
	assert(argc < 0x400);

	va_list argv;
	va_start(argv, argc);

	userspace_process process;
	process.pde = allocator_init_userspace_paging();
	process.real_entry_point = real_entry_point;
	process.argc = argc;
	process.argv = allocate_argv(process.pde, argc, argv);

	return process;
}


_Noreturn void userspace_start_process(userspace_process process) {
	assert(sizeof(sys_virtual_addr) == sizeof(void *));

	sys_page_directory_entry *pde = process.pde;
	sys_page_table_entry *stack_pte = allocator_calloc_page();
	pde[SYS_PD_SIZE - 1].page_table_addr = ((uint32_t)stack_pte) >> 12;
	pde[SYS_PD_SIZE - 1].us = 1;
	pde[SYS_PD_SIZE - 1].rw = 1;
	pde[SYS_PD_SIZE - 1].enabled = 1;

	uint32_t real_stack = (uint32_t)allocator_calloc_page();


	stack_pte[SYS_PD_SIZE - 1].frame_addr = (real_stack) >> 12;
	stack_pte[SYS_PD_SIZE - 1].us = 1;
	stack_pte[SYS_PD_SIZE - 1].rw = 1;
	stack_pte[SYS_PD_SIZE - 1].enabled = 1;

	real_stack += SYS_PAGE_SIZE - 16;
	*(uint32_t *)(real_stack + 4) = process.argc;
	*(uint32_t *)(real_stack + 8) = *(uint32_t*)(&process.argv);

	sys_virtual_addr stack_ptr;
	stack_ptr.offset = (uint32_t)real_stack & (uint32_t)0b111111111111;
	stack_ptr.page_table_index = SYS_PD_SIZE - 1;
	stack_ptr.page_directory_index = SYS_PD_SIZE - 1;

	sys_virtual_addr entry_point;
	entry_point.offset = 0;
	entry_point.page_directory_index = 1;
	entry_point.page_table_index = 0;


	sys_page_table_entry *entry_point_pte = allocator_calloc_page();

	pde[entry_point.page_directory_index].page_table_addr = ((uint32_t)entry_point_pte) >> 12;
	pde[entry_point.page_directory_index].enabled = 1;
	pde[entry_point.page_directory_index].us = 1;
	pde[entry_point.page_directory_index].rw = 1;

	for (size_t i = 0; i < USERSPACE_PACKAGE_SIZE; i++) {
		entry_point_pte[i].frame_addr = (process.real_entry_point >> 12) + i;
		entry_point_pte[i].enabled = 1;
		entry_point_pte[i].us = 1;
		entry_point_pte[i].rw = 1;
	}


	userspace_enter_context context;
	context.context.cs = SYS_GDT_USER_CODE;
	context.context.gs = SYS_GDT_USER_DATA;
	context.context.fs = SYS_GDT_USER_DATA;
	context.context.es = SYS_GDT_USER_DATA;
	context.context.ds = SYS_GDT_USER_DATA;
	context.context.eip = *(uint32_t*)(&entry_point);
	context.context.eflags = sys_read_eflags(); // https://wiki.osdev.org/CPU_Registers_x86#EFLAGS_Register
	context.context.eflags = resetbit(context.context.eflags, SYS_EFLAG_IOPL_0);
	context.context.eflags = resetbit(context.context.eflags, SYS_EFLAG_IOPL_1);
	context.context.eflags = setbit(context.context.eflags, SYS_EFLAG_IF);
	context.esp = *(uint32_t*)(&stack_ptr);
	context.ss = SYS_GDT_USER_DATA;

	sys_set_pde(pde);
	sys_enable_paging();
	sys_jump_to_userspace(&context);
}


static void userspace_exit(uint32_t status) {
	// userspace_start_process(userspace_process process);
	sys_infinite_loop();
}


_Noreturn void userspace_exit_forwarder(uint32_t status) {
	sys_disable_paging();
	if (status == -1) {
		printf("OOM!!!\n");
	} else {
		printf("\n-----------------------\n");
		printf("process exited with code: %d\n", status);
	}

	allocator_free_pde(get_cr3());

	userspace_exit(status);
}


void userspace_maybe_allocate_new_page(uint32_t cr2) {
	sys_page_directory_entry *pde = (sys_page_directory_entry *)get_cr3();
	sys_virtual_addr addr = *(sys_virtual_addr *)(&cr2);
	// kernel_panic("%x %x %x %x", addr.page_directory_index, addr.page_table_index, addr.offset, cr2);

	sys_disable_paging();

	if (!pde[addr.page_directory_index].enabled) {
		pde[addr.page_directory_index].page_table_addr = ((uint32_t)allocator_calloc_page()) >> 12;

		if (pde[addr.page_directory_index].page_table_addr == 0)
			userspace_exit_forwarder(-1);

		pde[addr.page_directory_index].us = 1;
		pde[addr.page_directory_index].rw = 1;
		pde[addr.page_directory_index].enabled = 1;

	}

	sys_page_table_entry *pte = (sys_page_table_entry *)(pde[addr.page_directory_index].page_table_addr << 12);


	if (!pte[addr.page_table_index].enabled) {
		pte[addr.page_table_index].frame_addr = ((uint32_t)allocator_alloc_page() >> 12);
		// kernel_panic("hello? %x", pte[addr.page_table_index].frame_addr);

		if (pte[addr.page_table_index].frame_addr == 0)
			userspace_exit_forwarder(-1);

		pte[addr.page_table_index].us = 1;
		pte[addr.page_table_index].rw = 1;
		pte[addr.page_table_index].enabled = 1;
	}

	sys_enable_paging();
}
