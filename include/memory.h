#pragma once

#include "dtypes.h"

void *memcpy(uint8_t *dest, const uint8_t *src, size_t n);
void *memmove(uint8_t *dest, const uint8_t *src, size_t n);
void *memset(uint8_t *s, uint8_t c, size_t n);
uint32_t setbit(uint32_t n, uint32_t i);
uint32_t resetbit(uint32_t n, uint32_t i);
uint32_t checkbit(uint32_t n, uint32_t i);
size_t strlen(char *str);
