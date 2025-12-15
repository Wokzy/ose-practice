#pragma once

#include "dtypes.h"

uint8_t sys_read_from_port(uint16_t port);
void sys_write_to_port(uint16_t port, uint8_t data);

void sys_cpu_halt();
void sys_fake_syscall();
void sys_infinite_loop();
void sys_zero_div();
void sys_break_gdt();
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
