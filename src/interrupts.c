
#include "interrupts.h"
#include "dtypes.h"
#include "printer.h"
#include "panic.h"
#include "assert.h"
#include "allocator.h"
#include "sys.h"

#define INTERRUPTS_TRAMPOLINE_SIZE 8
#define INTERRUPTS_TALBE_SIZE      256

static uint8_t int_with_error_code[8] = {0x8, 0xA, 0xB, 0xC, 0xD, 0xE, 0x11, 0x15, 0x1D, 0x1E};
static interrupts_config int_config;

void interrupts_collect_context();
void load_interrupt_descrtiptors_table(void *);

static void *gen_idt() {
	uint8_t *tramps = malloc_undead(INTERRUPTS_TRAMPOLINE_SIZE * INTERRUPTS_TALBE_SIZE, 8);

	for (size_t vector = 0; vector < INTERRUPTS_TALBE_SIZE; vector++) {
		bool has_error_code = false;
		for (uint8_t i = 0; i < 8; i++) {
			if (int_with_error_code[i] == vector) {
				has_error_code = true;
			}
		}

		uint8_t *tramp = tramps + vector * INTERRUPTS_TRAMPOLINE_SIZE;
		size_t offset = 0;
		if (!has_error_code) {
			tramp[offset++] = 0x50; // push eax
		}
		tramp[offset++] = 0x6A; // push imm8
		tramp[offset++] = vector; // vector
		tramp[offset++] = 0xE9; // relative jmp 32bit rel
		size_t distance = (size_t)interrupts_collect_context - (size_t)(tramp + offset + 4);
		*(size_t *)(tramp + offset) = distance;
	}

	interrupt_desc* idt = malloc_undead(sizeof(interrupt_desc) * INTERRUPTS_TALBE_SIZE, sizeof(interrupt_desc));

	for (size_t vector = 0; vector < INTERRUPTS_TALBE_SIZE; vector++) {
		idt[vector].offset_low = (size_t)(tramps + INTERRUPTS_TRAMPOLINE_SIZE * vector) & 0xFFFF;
		idt[vector].segment_selector = 0x10;
		idt[vector].reserved = 0;
		idt[vector].desc_type = 0b110 | int_config.is_trap_gate;
		idt[vector].D = 0b1;
		idt[vector].fixed_zero = 0b0;
		idt[vector].dpl = 0b00;
		idt[vector].P = 0b1;
		idt[vector].offset_high = ((size_t)(tramps + INTERRUPTS_TRAMPOLINE_SIZE * vector) >> 16) & 0xFFFF;
	}

	return idt;
}

#define MASTER_CMD_PORT  0x20
#define MASTER_DATA_PORT 0x21
#define SLAVE_CMD_PORT   0xA0
#define SLAVE_DATA_PORT  0xA1


static void delay_8259() {
	for (size_t i = 0; i < 4096; i++) {
		sys_write_to_port(0x80, 0b1);
	}
}

static void setup_8259() {
	sys_write_to_port(MASTER_CMD_PORT, 0b00010001);
	sys_write_to_port(SLAVE_CMD_PORT, 0b00010001);

	delay_8259();

	sys_write_to_port(MASTER_DATA_PORT, 0x20);
	sys_write_to_port(SLAVE_DATA_PORT, 0x28);

	delay_8259();

	sys_write_to_port(MASTER_DATA_PORT, 1 << 2);
	sys_write_to_port(SLAVE_DATA_PORT, 2);

	delay_8259();

	sys_write_to_port(MASTER_DATA_PORT, (int_config.auto_eoi << 1) | 1);
	sys_write_to_port(SLAVE_DATA_PORT, (int_config.auto_eoi << 1) | 1);

	delay_8259();

	sys_write_to_port(MASTER_DATA_PORT, 0xff);
	sys_write_to_port(SLAVE_DATA_PORT, 0xff);
}

void interrupts_send_eoi() {
	sys_write_to_port(MASTER_CMD_PORT, 0x20);
}


void interrupts_enable_device(sys_device device) {
	assert(device < 8);
	sys_write_to_port(MASTER_DATA_PORT, sys_read_from_port(MASTER_DATA_PORT) & ((uint8_t)(-1) ^ (1 << device)));
}

void interrupts_disable_device(sys_device device) {
	assert(device < 8);
	sys_write_to_port(MASTER_DATA_PORT, sys_read_from_port(MASTER_DATA_PORT) | (1 << device));
}

#undef MASTER_CMD_PORT
#undef MASTER_DATA_PORT
#undef SLAVE_CMD_PORT
#undef SLAVE_DATA_PORT

void interrupts_setup_interrupts(interrupts_config config) {
	assert(sizeof(interrupt_desc) == 8);
	int_config = config;
	void *idt = gen_idt();
	uint16_t idt_limit = INTERRUPTS_TALBE_SIZE * sizeof(interrupt_desc) - 1;
	uint64_t pseudo_idt = ((uint64_t)idt << 16) | idt_limit;
	load_interrupt_descrtiptors_table(&pseudo_idt);
	setup_8259();
}

#undef INTERRUPTS_TALBE_SIZE
#undef INTERRUPTS_TRAMPOLINE_SIZE

void interrupts_kernel_painc_handler(interrupt_context *context) {
	kernel_panic("unhandled interrupt #%x at %x:%x\n\n"
		  "Registers: \n"
		  "    EAX: %x" "    EBX: %x" "    ECX: %x" "    EDX: %x\n"
		  "    EDI: %x" "    ESI: %x" "    ESP: %x" "    EBP: %x\n"
		  "    DS : %x" "    ES : %x" "    GS : %x" "    FS : %x\n"
		  // "    XMM0: %x%x%x%x                                   \n"
		  // "    XMM1: %x%x%x%x                                   \n"
		  // "    XMM2: %x%x%x%x                                   \n"
		  // "    XMM3: %x%x%x%x                                   \n"
		  // "    XMM4: %x%x%x%x                                   \n"
		  // "    XMM5: %x%x%x%x                                   \n"
		  // "    XMM6: %x%x%x%x                                   \n"
		  // "    XMM7: %x%x%x%x                                   \n\n"
		  "Error code: %x\n\n"
		  "EFLAGS: %x\n", context->vector_index, context->cs, context->eip, context->eax, context->ebx, context->ecx, context->edx,
		  context->edi, context->esi, context->esp, context->ebp, context->ds, context->es, context->gs, context->fs,
		  // context->xmm0_0, context->xmm0_1, context->xmm0_2, context->xmm0_3,
		  // context->xmm1_0, context->xmm1_1, context->xmm1_2, context->xmm1_3,
		  // context->xmm2_0, context->xmm2_1, context->xmm2_2, context->xmm2_3,
		  // context->xmm3_0, context->xmm3_1, context->xmm3_2, context->xmm3_3,
		  // context->xmm4_0, context->xmm4_1, context->xmm4_2, context->xmm4_3,
		  // context->xmm5_0, context->xmm5_1, context->xmm5_2, context->xmm5_3,
		  // context->xmm6_0, context->xmm6_1, context->xmm6_2, context->xmm6_3,
		  // context->xmm7_0, context->xmm7_1, context->xmm7_2, context->xmm7_3,
		  context->error_code,
		  context->eflags);
}

void interrupts_interrupt_fowarder(interrupt_context *context) {
	int_config.int_handler(context);
}


void interrupts_setup_default_preset() {
	interrupts_config config = {
		.is_trap_gate = 0,
		.auto_eoi = 0,
		.int_handler = interrupts_kernel_painc_handler
	};

	interrupts_setup_interrupts(config);

	__asm__ volatile (
		".intel_syntax noprefix\n"
		"sti\n"
		".att_syntax\n"
	);
}

