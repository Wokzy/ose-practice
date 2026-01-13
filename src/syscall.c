
#include "sys.h"
#include "std.h"
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
			if (interrupts_is_user_space(context)) {
				feed_char(context->ecx, userspace_get_pid());
			} else {
				feed_char_system(context->ecx);
			}
			break;
		case 0x10:
			printf("%u ", context->ecx);
			break;
		default:
			break;
	}
}
