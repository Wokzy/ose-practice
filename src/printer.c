
#include <stdarg.h>

#include "vga.h"
#include "dtypes.h"
#include "printer.h"
#include "assert.h"
#include "memory.h"
#include "std.h"

#define PRINTER_MAX_X 80
#define PRINTER_MAX_Y 6

static size_t system_coord_x = 0;
static size_t system_coord_y = 0;

typedef struct {
	size_t coord_x;
	size_t coord_y;
} console;

static console process_consoles[4];


void printer_clear_screen() {
	vga_clear_screen();
	system_coord_x = 0;
	system_coord_y = 0;
}


void init_printer() {
	for (size_t i = 0; i < 4; i++) {
		process_consoles[i].coord_x = 0;
		process_consoles[i].coord_y = 0;
	}
	printer_clear_screen();
}

void feed_char_system(const char c) {
	if (c == '\r') {
		system_coord_x = 0;
		return;
	}
	if ((system_coord_x == PRINTER_MAX_X) || (c == '\n')) {
		system_coord_x = 0;
		if (system_coord_y == 24) {
			vga_linefeed(-1);
		} else {
			system_coord_y++;
		}
	}
	assert(2 == 3);

	if (c != '\n') {
		vga_print_char(system_coord_x, system_coord_y, c);
		system_coord_x++;
	}
}


void feed_char(const char c, uint32_t pid) {
	if (c == '\r') {
		process_consoles[pid].coord_x = 0;
		return;
	}
	if ((process_consoles[pid].coord_x == PRINTER_MAX_X) || (c == '\n')) {
		process_consoles[pid].coord_x = 0;
		if (process_consoles[pid].coord_y == PRINTER_MAX_Y) {
			vga_linefeed(pid);
		} else {
			process_consoles[pid].coord_y++;
		}
	}

	if (c != '\n') {
		vga_print_char(process_consoles[pid].coord_x, 6 * pid + process_consoles[pid].coord_y, c);
		process_consoles[pid].coord_x++;
	}
}



