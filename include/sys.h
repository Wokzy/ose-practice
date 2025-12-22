#pragma once

#include "dtypes.h"

uint8_t sys_read_from_port(uint16_t port);
void sys_write_to_port(uint16_t port, uint8_t data);

void sys_cpu_halt();
void sys_fake_syscall();
void sys_infinite_loop();
void sys_zero_div();
void sys_break_gdt();
void sys_init_paging(void *pde_ptr);
void sys_enable_paging();
void sys_disable_paging();
void sys_jump_to_userspace(void *);
uint32_t sys_read_eflags();

typedef enum {
	TIMER,
	KEYBOARD,
} sys_device;


#pragma pack(push, 1)
typedef struct {
	uint16_t limit_0_15      : 16;
	uint16_t base_0_15       : 16;
	uint8_t  base_16_23      : 8;
	uint8_t  desc_type       : 3;
	uint8_t  S               : 1;
	uint8_t  DPL             : 2;
	uint8_t  P               : 1;
	uint16_t limit_16_19     : 4;
	uint16_t AVL             : 1;
	uint16_t zero            : 1;
	uint16_t d_b             : 1;
	uint16_t g               : 1;
	uint16_t base_24_31      : 1;
} gdt_desc;
#pragma pack(pop)


#pragma pack(push, 1)
typedef struct {
	uint8_t enabled          : 1;
	uint8_t rw               : 1;
	uint8_t us               : 1;
	uint8_t __unused2        : 4;
	uint8_t is_huge_page     : 1; // (0/1) (table/huge page)
	uint8_t __unused         : 4;
	uint32_t page_table_addr : 20;
} sys_page_directory_entry;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
	uint8_t  enabled         : 1;
	uint8_t  rw              : 1;
	uint8_t  us              : 1;
	uint16_t __unused        : 9;
	uint32_t frame_addr      : 20;
} sys_page_table_entry;
#pragma pack(pop)


#pragma pack(push, 1)
typedef struct {
	uint16_t offset               : 12;
	uint16_t page_table_index     : 10;
	uint16_t page_directory_index : 10;
} sys_virtual_addr;
#pragma pack(pop)


#define SYS_EFLAG_IOPL_0 12
#define SYS_EFLAG_IOPL_1 13
#define SYS_EFLAG_IF 9

#define SYS_GDT_USER_CODE 0x23
#define SYS_GDT_USER_DATA 0x1b
