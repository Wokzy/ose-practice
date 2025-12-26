
#include "sys.h"
#include "dtypes.h"
#include "assert.h"
#include "syscall.h"
#include "printer.h"
#include "userspace.h"
#include "interrupts.h"

// eax, ecx, edx

void syscall(interrupt_context *context) {
	switch (context->eax) {
		case 0:
			userspace_exit_forwarder(context->ecx);
			break;
		case 1:
			feed_char(context->ecx);
			break;
		case 0x10:
			printf("%u ", context->ecx);
			break;
		default:
			break;
	}
}
