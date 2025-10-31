
#include <stdarg.h>

#include "vga.h"
#include "dtypes.h"
#include "printer.h"
#include "memory.h"

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


static void feed_char(const char c) {
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


static void print_unsigned(uint32_t number) {
	if (number == 0) {
		feed_char('0');
		return;
	}

	char value[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

	for (size_t i = 10; i > 0; i--) {
		if (number == 0)
			break;

		value[i - 1] = '0' + (char)(number % 10);
		number /= 10;
	}

	for (size_t i = 0; i < 10; i++) {
		if (value[i] != -1)
			feed_char(value[i]);
	}
}


static void print_signed(int32_t number) {
	if (number < 0){
		feed_char('-');
		number *= -1;
	}

	print_unsigned((uint32_t)number);
}


static void print_hex(uint32_t number) {
	// if (number == 0) {
	// 	feed_char('0');
	// 	return;
	// }

	char value[8] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

	for (size_t i = 8; i > 0; i--) {
		// if (number == 0)
		// 	break;

		char tmp = (char)(number % 16);
		value[i - 1] = (tmp < 10) ? '0' + tmp : 'a' + tmp - (char)10;
		number >>= 4;
	}

	for (size_t i = 0; i < 8; i++) {
		if (value[i] != -1)
			feed_char(value[i]);
	}
}


void vprintf(const char *str, va_list args) {
	if (str == NULL)
		return;

	size_t idx = 0;
	while (str[idx] != 0) {
		if (str[idx] == '%') {
			idx++;
			if (str[idx] == '%') {
				feed_char('%');
			}
			else if (str[idx] == 'd') {
				print_signed(va_arg(args, int32_t));
			} else if (str[idx] == 'u') {
				print_unsigned(va_arg(args, uint32_t));
			} else if (str[idx] == 's') {
				printf(va_arg(args, char *));
			} else if (str[idx] == 'c') {
				feed_char(va_arg(args, char));
			} else if (str[idx] == 'x') {
				print_hex(va_arg(args, uint32_t));
			}
		} else {
			feed_char(str[idx]);
		}

		idx++;
	}
	// feed_char('\n');
}


void printf(const char *str, ...) {
	if (str == NULL)
		return;

	va_list args;

	va_start(args, str);
	vprintf(str, args);
}

