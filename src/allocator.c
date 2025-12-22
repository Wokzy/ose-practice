
#include "sys.h"
#include "dtypes.h"
#include "panic.h"
#include "memory.h"
#include "assert.h"

#define UNDEAD_PTR_INIT (size_t)0x100000
#define UNDEAD_SIZE (size_t)0x10000


static size_t undead_ptr = UNDEAD_PTR_INIT;

void *malloc_undead(size_t size, size_t alignment) {
	size_t target = (undead_ptr % alignment) > 0 ? undead_ptr + (alignment - (undead_ptr % alignment)) : undead_ptr;
	assert((target % alignment) == 0);

	if (target + size > UNDEAD_PTR_INIT + UNDEAD_SIZE) {
		kernel_panic("out of memory for undead objects");
	}

	undead_ptr = target + size;
	return (void *)target;
}


void *calloc_undead(size_t size, size_t alignment) {
	void *ptr = malloc_undead(size, alignment);
	memset((uint8_t *)ptr, 0, size);
	return ptr;
}

static size_t free_page_ptr = 0;
static size_t free_page_counter = SYS_PAGE_ARENA_AMOUNT;

static inline void init_page_arena() {
	size_t page_ptr = SYS_PAGE_ARENA_AMOUNT * SYS_PAGE_SIZE + SYS_PAGE_PTR_INIT;
	for (; page_ptr > SYS_PAGE_PTR_INIT; page_ptr -= SYS_PAGE_SIZE)
		*(((void **)(page_ptr - SYS_PAGE_SIZE))) = (void *)page_ptr;

	assert(page_ptr == SYS_PAGE_PTR_INIT);
	free_page_ptr = SYS_PAGE_PTR_INIT;
}

void *allocator_alloc_page() {
	if (free_page_counter == 0)
		kernel_panic("no free memory pages left");

	if (free_page_ptr == 0)
		init_page_arena();

	void **res = (void *)free_page_ptr;
	free_page_ptr = (size_t) (*res);
	free_page_counter--;

	memset((uint8_t *)res, 0, SYS_PAGE_SIZE);

	return (void *)res;
}

void allocator_free_page(void *ptr) {
	void **old_ptr = (void **)ptr;
	*old_ptr = (void *)free_page_ptr;
	free_page_ptr = (size_t)old_ptr;
	free_page_counter++;
}


sys_page_directory_entry *allocator_init_userspace_paging() {
	assert(sizeof(sys_page_directory_entry) == 4);
	assert(sizeof(sys_page_table_entry) == 4);

	sys_page_directory_entry *pde_ptr = (sys_page_directory_entry *)allocator_alloc_page();
	sys_page_table_entry *page_table_addr = (sys_page_table_entry *)allocator_alloc_page();

	for (uint32_t i = 0; i < SYS_PD_SIZE; i++) {
		page_table_addr[i].frame_addr = i;
		page_table_addr[i].us = ((i <= 0x80) || (i >= 0x100)); // for vga protection
		page_table_addr[i].rw = 1;
		page_table_addr[i].enabled = 1;
	}

	pde_ptr[0].page_table_addr = ((uint32_t)page_table_addr) >> 12;
	pde_ptr[0].us = 1;
	pde_ptr[0].rw = 1;
	pde_ptr[0].enabled = 1;

	return pde_ptr;
}


void mmap_simple() {
	sys_disable_paging();
}


#undef UNDEAD_PTR_INIT
#undef UNDEAD_SIZE
