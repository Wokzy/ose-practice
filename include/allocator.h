#pragma once

#include "dtypes.h"

void *malloc_undead(size_t size, size_t alignment);
void *calloc_undead(size_t size, size_t alignment);

void *allocator_alloc_page();
void *allocator_calloc_page();
void allocator_free_page(void *ptr);

sys_page_directory_entry *allocator_init_userspace_paging();
void *allocator_free_pde(sys_page_directory_entry *pde);
