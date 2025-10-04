
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
	memcpy(dest, src, n);
	// uint8_t tmp = 0;

	// for (size_t i = 0; i < n; i++) {
	// 	tmp = src[i];
	// 	dest[i] = tmp;
	// }

	// return dest;
}


void *memset(uint8_t *s, uint8_t c, size_t n) {

	#pragma vector
	for (size_t i = 0; i < n; i++)
		s[i] = c;

	return s;
}
