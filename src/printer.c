
#include <stdarg.h>

#include "vga.h"
#include "dtypes.h"
#include "printer.h"
#include "memory.h"
#include "std.h"

#define PRINTER_MAX_X 80
#define PRINTER_MAX_Y 24

static size_t coord_x = 0;
static size_t coord_y = 0;


void printer_clear_screen() {
	vga_clear_screen();
	coord_x = 0;
	coord_y = 0;
}


void init_printer() {
	printer_clear_screen();
}


void feed_char(const char c) {
	if (c == '\r') {
		coord_x = 0;
		return;
	}
	if ((coord_x == PRINTER_MAX_X) || (c == '\n')) {
		coord_x = 0;
		if (coord_y == PRINTER_MAX_Y) {
			vga_linefeed();
		} else {
			coord_y++;
		}
	}

	if (c != '\n') {
		vga_print_char(coord_x, coord_y, c);
		coord_x++;
	}
}



