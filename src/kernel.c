
#include "dtypes.h"

void cpu_halt();

void kernel_entry() {
	*((uint16_t*) 0xB8000) = 0;
	cpu_halt();
}
