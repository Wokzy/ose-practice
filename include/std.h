
#pragma once
#include "dtypes.h"
#include "printer.h"

void std_print_char(const char c);

void vprintf(const char *str, va_list args);
void printf(const char *str, ...);
