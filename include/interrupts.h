#pragma once

struct interrupt_context;

void interrupts_setup_interrupts();
void interrupts_universal_handler(struct interrupt_context* context);
