
#include "memory.h"
#include "assert.h"
#include "dtypes.h"


void *memcpy(uint8_t *dest, const uint8_t *src, size_t n) {

	#pragma vector
	for (size_t i = 0; i < n; i++)
		dest[i] = src[i];

	return dest;
}


void *memmove(uint8_t *dest, const uint8_t *src, size_t n) {
	if (((size_t)dest < (size_t)src) || ((size_t)src - (size_t)dest > n)) {
		return memcpy(dest, src, n);
	}

	for (size_t i = n; i > 0; --i) {
		dest[i - 1] = src[i - 1];
	}

	return dest;
}


void *memset(uint8_t *s, uint8_t c, size_t n) {

	#pragma vector
	for (size_t i = 0; i < n; i++)
		s[i] = c;

	return s;
}


uint32_t setbit(uint32_t n, uint32_t i) {
	return n | (1 << i);
}

uint32_t resetbit(uint32_t n, uint32_t i) {
	return n & (~(1 << i));
}

uint32_t checkbit(uint32_t n, uint32_t i) {
	return (n & (1 << i)) > 0;
}


size_t strlen(char *str) {
	uint32_t len = 0;
	while (str[len] != 0) {
		len++;
		assert(len < 0x1000);
	}

	return len;
}
