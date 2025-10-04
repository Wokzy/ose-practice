#pragma once

#include "dtypes.h"

enum VGA_COLOR {
	BLACK,
	BLUE,
	GREEN,
	CYAN,
	RED,
	PURPLE,
	BROWN,
	GRAY,
	LIGHT_GRAY,
	LIGHT_BLUE,
	LIGHT_GREEN,
	LIGHT_CYAN,
	LIGHT_RED,
	LIGHT_PURPLE,
	YELLOW,
	WHITE,
};

void vga_clear_screen(void);
void vga_init(void);
void vga_print_colored_char(uint8_t x, uint8_t y, uint8_t chr, enum VGA_COLOR front, enum VGA_COLOR background, bool blinking);
void vga_print_char(uint8_t x, uint8_t y, uint8_t chr);
void vga_linefeed(void);

