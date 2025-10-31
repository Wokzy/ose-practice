
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

	if (target > UNDEAD_PTR_INIT + UNDEAD_SIZE) {
		kernel_panic("out of memory for undead objects");
	}

	undead_ptr = target + size;
	// kernel_panic("%x\n", undead_ptr);
	return (void *)target;
}


void *calloc_undead(size_t size, size_t alignment) {
	void *ptr = malloc_undead(size, alignment);
	memset((uint8_t *)ptr, 0, size);
	return ptr;
}

