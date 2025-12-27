
#include "vga.h"
#include "sys.h"
#include "std.h"
#include "assert.h"
#include "dtypes.h"
#include "memory.h"
#include "printer.h"
#include "allocator.h"
#include "userspace.h"
#include "interrupts.h"


_Noreturn void kernel_entry() {
	init_printer();
	interrupts_setup_default_preset();
	userspace_setup_userspace();

	uint32_t pid = userspace_init_process(0x20000, 4, "hello", "baz", "foo", "123");
	uint32_t pid2 = userspace_init_process(0x22000, 4, "hello", "baz", "foo", "123");
	uint32_t pid3 = userspace_init_process(0x21000, 4, "hello", "baz", "foo", "123");
	userspace_start_process(pid);

	sys_infinite_loop();
	// sys_cpu_halt();
}
