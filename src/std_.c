
#include "dtypes.h"
#include "std.h"


static void print_unsigned(uint32_t number) {
	if (number == 0) {
		std_print_char('0');
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
			std_print_char(value[i]);
	}
}


static void print_signed(int32_t number) {
	if (number < 0){
		std_print_char('-');
		number *= -1;
	}

	print_unsigned((uint32_t)number);
}


static void print_hex(uint32_t number) {
	char value[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

	for (size_t i = 8; i > 0; i--) {

		char tmp = (char)(number % 16);
		value[i - 1] = (tmp < 10) ? '0' + tmp : 'a' + tmp - (char)10;
		number >>= 4;
	}

	for (size_t i = 0; i < 8; i++) {
		if (value[i] != -1)
			std_print_char(value[i]);
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
				std_print_char('%');
			}
			else if (str[idx] == 'd') {
				print_signed(va_arg(args, int32_t));
			} else if (str[idx] == 'u') {
				print_unsigned(va_arg(args, uint32_t));
			} else if (str[idx] == 's') {
				printf(va_arg(args, char *));
			} else if (str[idx] == 'c') {
				std_print_char(va_arg(args, char));
			} else if (str[idx] == 'x') {
				print_hex(va_arg(args, uint32_t));
			}
		} else {
			std_print_char(str[idx]);
		}

		idx++;
	}
}


void printf(const char *str, ...) {
	if (str == NULL)
		return;

	va_list args;

	va_start(args, str);
	vprintf(str, args);
}
