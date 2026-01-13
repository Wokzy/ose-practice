
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


static userspace_process processes[4];
static uint32_t current_pid = 0;
static uint16_t entered_userspace = 0;


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


void userspace_setup_userspace() {
	for (size_t i = 0; i < 4; i++) {
		processes[i].pid = -1;
	}
}


uint32_t userspace_init_process(uint32_t real_entry_point, uint32_t argc, ...) {
	assert(sizeof(sys_virtual_addr) == sizeof(void *));
	assert(argc < 0x400);

	va_list argv;
	va_start(argv, argc);

	uint32_t pid = -1;

	for (uint32_t i = 0; i < 4; i++) {
		if (processes[i].pid == -1) {
			processes[i].pde = allocator_init_userspace_paging();
			processes[i].argc = argc;
			processes[i].argv = allocate_argv(processes[i].pde, argc, argv);
			processes[i].pid = i;
			pid = i;
			break;
		}
	}

	assert(pid != -1);

	sys_page_directory_entry *pde = processes[pid].pde;
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
	*(uint32_t *)(real_stack + 4) = processes[pid].argc;
	*(uint32_t *)(real_stack + 8) = *(uint32_t*)(&processes[pid].argv);

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
		entry_point_pte[i].frame_addr = (real_entry_point >> 12) + i;
		entry_point_pte[i].enabled = 1;
		entry_point_pte[i].us = 1;
		entry_point_pte[i].rw = 1;
	}


	// interrupt_context context;
	processes[pid].context.cs = SYS_GDT_USER_CODE;
	processes[pid].context.gs = SYS_GDT_USER_DATA;
	processes[pid].context.fs = SYS_GDT_USER_DATA;
	processes[pid].context.es = SYS_GDT_USER_DATA;
	processes[pid].context.ds = SYS_GDT_USER_DATA;
	processes[pid].context.eip = *(uint32_t*)(&entry_point);
	processes[pid].context.eflags = sys_read_eflags(); // https://wiki.osdev.org/CPU_Registers_x86#EFLAGS_Register
	processes[pid].context.eflags = resetbit(processes[pid].context.eflags, SYS_EFLAG_IOPL_0);
	processes[pid].context.eflags = resetbit(processes[pid].context.eflags, SYS_EFLAG_IOPL_1);
	processes[pid].context.eflags = setbit(processes[pid].context.eflags, SYS_EFLAG_IF);
	processes[pid].context.esp_caller = *(uint32_t*)(&stack_ptr);
	processes[pid].context.ss = SYS_GDT_USER_DATA;

	return pid;
}


static void goto_next_process() {
	entered_userspace = 1;
	sys_disable_paging();
	for (uint32_t i = 1;;i++) {
		if (processes[(current_pid + i) % 4].pid != -1) {
			current_pid = (current_pid + i) % 4;
			break;
		}
	}

	sys_set_pde(processes[current_pid].pde);
	sys_enable_paging();
	sys_jump_to_userspace(&processes[current_pid].context);
}

_Noreturn void userspace_goto_userspace() {
	assert(sizeof(sys_virtual_addr) == sizeof(void *));

	// current_pid = pid;
	// entered_userspace = 1;

	goto_next_process();

	// sys_set_pde(pde);
	// sys_enable_paging();
	// sys_jump_to_userspace(&context);
}


static void userspace_exit(uint32_t status) {
	// userspace_start_process(userspace_process process);
	processes[current_pid].pid = -1;
	goto_next_process();
}


_Noreturn void userspace_exit_forwarder(uint32_t status) {
	sys_disable_paging();
	if (status == -1) {
		printf("Segmentation fault: core dumped\n");
	} else if (status == -2) {
		printf("Stack overflow\n");
	} else {
		printf("\n-----------------------\n");
		printf("process exited with code: %d\n", status);
	}

	allocator_free_pde(get_cr3());

	userspace_exit(status);
}


void userspace_timer_handler(interrupt_context *context) {
	processes[current_pid].context = *context;
	goto_next_process();
}


void userspace_maybe_allocate_new_page(uint32_t cr2) {
	sys_page_directory_entry *pde = (sys_page_directory_entry *)get_cr3();
	sys_virtual_addr addr = *(sys_virtual_addr *)(&cr2);

	sys_disable_paging();

	if (addr.page_directory_index == SYS_PD_SIZE - 2) {
		userspace_exit_forwarder(-2);
	} else if (addr.page_directory_index != SYS_PD_SIZE - 1) {
		userspace_exit_forwarder(-1);
	}

	printf("%x\n", cr2);


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

		if (pte[addr.page_table_index].frame_addr == 0)
			userspace_exit_forwarder(-1);

		pte[addr.page_table_index].us = 1;
		pte[addr.page_table_index].rw = 1;
		pte[addr.page_table_index].enabled = 1;
	}

	sys_enable_paging();
}

uint32_t userspace_get_pid() {
	return current_pid;
}

uint16_t userspace_in_user_space() {
	return entered_userspace;
}
