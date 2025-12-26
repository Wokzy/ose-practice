
#include "vga.h"
#include "sys.h"
#include "exp.h"
#include "assert.h"
#include "dtypes.h"
#include "memory.h"
#include "printer.h"
#include "allocator.h"
#include "userspace.h"
#include "interrupts.h"



void user_function() {
	printf("from user!!!!\n");
	sys_infinite_loop();
}

_Noreturn void kernel_entry() {
	init_printer();
	// interrupts_setup_default_preset();
	// allocator_init_paging();

	// void *user_stack = (void *)((uint32_t)malloc_undead(4096, 1) + (uint32_t)4095);
	// userspace_enter_userspace(exp_7, user_stack);

	exp_10_init();

	// printf("HELLO!!!");

	// uint8_t *addr1 = allocator_alloc_page();
	// uint8_t *addr2 = allocator_alloc_page();
	// uint8_t *addr3 = allocator_alloc_page();
	// uint8_t *addr4 = allocator_alloc_page();
	// printf("addr1 %x\n", addr1);
	// printf("addr2 %x\n", addr2);
	// printf("addr3 %x\n", addr3);
	// printf("addr4 %x\n", addr4);

	// addr4[4] = 123;

	// printf("%x\n", addr4[4]);

	// allocator_free_page(addr2);
	// allocator_free_page(addr3);

	// void *addr5 = allocator_alloc_page();

	// printf("addr5 %x\n", addr5);

	// void *ptr = allocator_alloc_page();
	// for (;;) {
	// 	printf("%x\n", ptr);
	// 	ptr = allocator_alloc_page();
	// }

	sys_infinite_loop();
	// sys_cpu_halt();
}
