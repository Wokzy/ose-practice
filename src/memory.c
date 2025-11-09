
#include "memory.h"
#include "dtypes.h"


void *memcpy(uint8_t *dest, const uint8_t *src, size_t n) {

	#pragma vector
	for (size_t i = 0; i < n; i++)
		dest[i] = src[i];

	return dest;
}


void *memmove(uint8_t *dest, const uint8_t *src, size_t n) {
	// TODO: support real memove

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
