#pragma once

#include <stdarg.h>

void printer_clear_screen();
void init_printer();
void vprintf(const char *str, va_list args);
void printf(const char *str, ...);
