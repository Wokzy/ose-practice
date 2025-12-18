#pragma once

#include "dtypes.h"

void *malloc_undead(size_t size, size_t alignment);
void *calloc_undead(size_t size, size_t alignment);

void *allocator_alloc_page();
void allocator_free_page(void *ptr);

void allocator_init_paging();
