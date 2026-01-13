#pragma once

#include <stdarg.h>

void feed_char_system(const char c);
void feed_char(const char c, uint32_t pid);
void printer_clear_screen();
void init_printer();
