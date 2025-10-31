
#include "interrupts.h"
#include "dtypes.h"
#include "printer.h"
#include "panic.h"
#include "assert.h"
#include "allocator.h"

#define INTERRUPTS_TRAMPOLINE_SIZE 8
#define INTERRUPTS_TALBE_SIZE 256

static uint8_t int_with_error_code[8] = {0x8, 0xA, 0xB, 0xC, 0xD, 0xE, 0x11, 0x15};

extern void interrupts_collect_context(); // in boot.asm
extern void load_interrupt_descrtiptors_table(void *); // in boot.asm

typedef struct {
	uint16_t offset_low      : 16;
	uint16_t segment_selector: 16;
	uint8_t  reserved        : 8;
	uint8_t  desc_type       : 3;
	uint8_t  D               : 1;
	uint8_t  fixed_zero      : 1;
	uint8_t  dpl             : 2;
	uint8_t  P               : 1;
	uint16_t offset_high     : 16;
} interrupt_desc;


struct interrupt_context {
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	// uint32_t xmm0_0, xmm0_1, xmm0_2, xmm0_3;
	alignas(4) uint16_t gs, fs, es, ds;
	alignas(4) uint8_t vector_index;
	uint32_t error_code;
	uint32_t eip;
	alignas(4) uint16_t cs;
	uint32_t eflags;
};


static void* gen_idt() {
	uint8_t* tramps = malloc_undead(INTERRUPTS_TRAMPOLINE_SIZE * INTERRUPTS_TALBE_SIZE, 8);

	for (size_t vector = 0; vector < INTERRUPTS_TALBE_SIZE; vector++) {
		bool has_error_code = false;
		for (uint8_t i = 0; i < 8; i++) {
			if (int_with_error_code[i] == vector) {
				has_error_code = true;
			}
		}

		uint8_t* tramp = tramps + vector * INTERRUPTS_TRAMPOLINE_SIZE;
		size_t offset = 0;
		if (!has_error_code) {
			tramp[offset++] = 0x50; // push eax
		}
		tramp[offset++] = 0x6A; // push imm8
		tramp[offset++] = vector; // vector
		tramp[offset++] = 0xE9; // relative jmp 16bit rel
		size_t distance = (size_t)interrupts_collect_context - (size_t)(tramp + offset + 4);
		*(size_t*)(tramp + offset) = distance;
	}

	interrupt_desc* idt = malloc_undead(sizeof(interrupt_desc) * INTERRUPTS_TALBE_SIZE, sizeof(interrupt_desc)); // slide 37

	for (size_t vector = 0; vector < INTERRUPTS_TALBE_SIZE; vector++) {
		idt[vector].offset_low = (size_t)(tramps + INTERRUPTS_TRAMPOLINE_SIZE * vector) & 0xFFFF;
		idt[vector].segment_selector = 0x10;
		idt[vector].reserved = 0;
		idt[vector].desc_type = 0b110;
		idt[vector].D = 0b1;
		idt[vector].fixed_zero = 0b0;
		idt[vector].dpl = 0b00;
		idt[vector].P = 0b1;
		idt[vector].offset_high = ((size_t)(tramps + INTERRUPTS_TRAMPOLINE_SIZE * vector) >> 16) & 0xFFFF;
	}

	return idt;
}

void setup_interrupts() {
	assert(sizeof(interrupt_desc) == 8);
	void* idt = gen_idt();
	uint16_t idt_limit = INTERRUPTS_TALBE_SIZE * sizeof(interrupt_desc) - 1;
	uint64_t pseudo_idt = ((uint64_t)idt << 16) | idt_limit;
	load_interrupt_descrtiptors_table(&pseudo_idt);
	// __asm__ volatile (
	// 	".intel_syntax noprefix\n"
	// 	"lidt [eax]\n"
	// 	".att_syntax\n"
	// 	:
	// 	: "a" (&pseudo_idt)
	// );
}

void universal_handler(struct interrupt_context* context) {
	kernel_panic("unhandled interrupt #%x at %x:%x\n\n"
		  "Registers: \n"
		  "    EAX: %x" "    EBX: %x" "    ECX: %x" "    EDX: %x\n"
		  "    EDI: %x" "    ESI: %x" "    ESP: %x" "    EBP: %x\n"
		  "    DS : %x" "    ES : %x" "    GS : %x" "    FS : %x\n\n"
		  // "    XMM0: %x%x%x%x                                   \n\n"
		  "Error code: %x\n\n"
		  "EFLAGS: %x\n", context->vector_index, context->cs, context->eip, context->eax, context->ebx, context->ecx, context->edx,
		  context->edi, context->esi, context->esp, context->ebp, context->ds, context->es, context->gs, context->fs,
		  // context->xmm0_0, context->xmm0_1, context->xmm0_2, context->xmm0_3,
		  context->error_code,
		  context->eflags);
}
