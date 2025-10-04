
#include "dtypes.h"
#include "memory.h"

void cpu_halt();

void *vga = (void*)0xB8000;

void kernel_entry() {

	memset(vga, 0, 4000);
	*((uint16_t*) 0xB8000) = 0x748;
	cpu_halt();
}
