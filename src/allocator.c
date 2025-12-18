
#include "sys.h"
#include "dtypes.h"
#include "panic.h"
#include "memory.h"
#include "assert.h"

#define UNDEAD_PTR_INIT (size_t)0x100000
#define UNDEAD_SIZE (size_t)0x10000

#define PAGE_SIZE (size_t)0x1000
#define PAGE_PTR_INIT (size_t)0x400000
#define PAGE_ARENA_AMOUNT (size_t)0x8000 // 128 Mb

static size_t undead_ptr = UNDEAD_PTR_INIT;

void *malloc_undead(size_t size, size_t alignment) {
	size_t target = (undead_ptr % alignment) > 0 ? undead_ptr + (alignment - (undead_ptr % alignment)) : undead_ptr;
	assert((target % alignment) == 0);

	if (target > UNDEAD_PTR_INIT + UNDEAD_SIZE) {
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
static size_t free_page_counter = PAGE_ARENA_AMOUNT;

static inline void init_page_arena() {
	size_t page_ptr = PAGE_ARENA_AMOUNT * PAGE_SIZE + PAGE_PTR_INIT;
	for (; page_ptr > PAGE_PTR_INIT; page_ptr -= PAGE_SIZE)
		*(((void **)(page_ptr - PAGE_SIZE))) = (void *)page_ptr;

	assert(page_ptr == PAGE_PTR_INIT);
	free_page_ptr = PAGE_PTR_INIT;
}

void *allocator_alloc_page() {
	if (free_page_counter == 0)
		kernel_panic("no free memory pages left");

	if (free_page_ptr == 0)
		init_page_arena();

	void **res = (void *)free_page_ptr;
	free_page_ptr = (size_t) (*res);
	free_page_counter--;

	memset((uint8_t *)res, 0, PAGE_SIZE);

	return (void *)res;
}

void allocator_free_page(void *ptr) {
	void **old_ptr = (void **)ptr;
	*old_ptr = (void *)free_page_ptr;
	free_page_ptr = (size_t)old_ptr;
	free_page_counter++;
}


void allocator_init_paging() {
	uint32_t *pde_ptr = 0;

	pde_ptr = (uint32_t *)calloc_undead(0x400 * 4, 1);
	pde_ptr[0] = setbit(pde_ptr[0], 0); // enable page
	pde_ptr[0] = setbit(pde_ptr[0], 1); // enable read/write
	pde_ptr[0] = setbit(pde_ptr[0], 2); // US bit
	pde_ptr[0] = setbit(pde_ptr[0], 7); // PS

	sys_enable_paging(pde_ptr);
}


#undef UNDEAD_PTR_INIT
#undef UNDEAD_SIZE
#undef PAGES_PTR_INIT
#undef PAGES_SIZE
#undef PAGE_ARENA_AMOUNT

