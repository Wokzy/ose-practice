
#include "sys.h"
#include "dtypes.h"
#include "assert.h"
#include "syscall.h"
#include "printer.h"
#include "userspace.h"
#include "interrupts.h"

void syscall(interrupt_context *context) {
	switch (context->eax) {
		case 0:
			userspace_exit_forwarder(context->ebx);
			break;
		case 0x10:
			printf("%u ", context->ebx);
			break;
		default:
			break;
	}
}
