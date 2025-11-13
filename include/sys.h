#pragma once

#include "dtypes.h"

uint8_t sys_read_from_port(uint16_t port);
void sys_write_to_port(uint16_t port, uint8_t data);

void sys_cpu_halt();
void sys_fake_syscall();
void sys_infinite_loop();
void sys_zero_div();

typedef enum {
	TIMER,
	KEYBOARD,
} sys_device;
