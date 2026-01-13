
#pragma once
#include "dtypes.h"
#include "printer.h"

void std_print_char(const char c);
void std_n_rec(uint32_t n);

void vprintf(const char *str, va_list args);
void printf(const char *str, ...);
