#pragma once

#include "dtypes.h"

#pragma pack(push, 1)
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
#pragma pack(pop)


struct interrupt_context {
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t xmm7_0, xmm7_1, xmm7_2, xmm7_3;
	uint32_t xmm6_0, xmm6_1, xmm6_2, xmm6_3;
	uint32_t xmm5_0, xmm5_1, xmm5_2, xmm5_3;
	uint32_t xmm4_0, xmm4_1, xmm4_2, xmm4_3;
	uint32_t xmm3_0, xmm3_1, xmm3_2, xmm3_3;
	uint32_t xmm2_0, xmm2_1, xmm2_2, xmm2_3;
	uint32_t xmm1_0, xmm1_1, xmm1_2, xmm1_3;
	uint32_t xmm0_0, xmm0_1, xmm0_2, xmm0_3;
	alignas(4) uint16_t gs, fs, es, ds;
	alignas(4) uint8_t vector_index;
	uint32_t error_code;
	uint32_t eip;
	alignas(4) uint16_t cs;
	uint32_t eflags;
};

struct interrupts_config {
	uint8_t is_trap_gate;
	void (*int_handler) (struct interrupt_context *);
};

void interrupts_setup_interrupts(struct interrupts_config config);
void interrupts_kernel_painc_handler(struct interrupt_context *context);
void interrupts_universal_handler(struct interrupt_context* context);
