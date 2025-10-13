#pragma once

#include <stdarg.h>

void vkernel_panic(const char* msg, va_list args);
void kernel_panic(const char* msg, ...);
