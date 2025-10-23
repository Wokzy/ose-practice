
#include "vga.h"
#include "dtypes.h"
#include "memory.h"

#define VGA_LINE_LEN 160
#define VGA_SCREEN_SIZE 4000

static size_t screen = (size_t)0xB8000;


static inline uint8_t *vga_coords_to_ptr(uint8_t x, uint8_t y) {
	return (uint8_t *)(screen + 2 * (80 * y + x));
}

void vga_clear_screen(void) {
	memset(vga_coords_to_ptr(0, 0), 0, VGA_SCREEN_SIZE);
}

void vga_init(void) {
	vga_clear_screen();
}

void vga_print_colored_char(uint8_t x, uint8_t y, uint8_t chr, enum VGA_COLOR front, enum VGA_COLOR background, bool blinking) {
	uint8_t *dest = vga_coords_to_ptr(x, y);

	uint16_t output = (uint16_t)(((uint8_t) background << 4) | (uint8_t)front) << 8;
	output |= (uint16_t)chr;

	if (blinking) {
		output |= (uint16_t)128;
	}

	*dest = output;
}


void vga_print_char(uint8_t x, uint8_t y, uint8_t chr) {
	vga_print_colored_char(x, y, chr, WHITE, BLACK, false);
}

void vga_linefeed(void) {
	memmove(vga_coords_to_ptr(0, 0), vga_coords_to_ptr(0, 1), VGA_SCREEN_SIZE - VGA_LINE_LEN);
	memset(vga_coords_to_ptr(0, 79), 0, VGA_LINE_LEN);
}



