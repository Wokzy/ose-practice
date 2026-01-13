
#include "std.h"

int main(int argc, char** argv) {
	for (int i = 0; i < argc; i++) {
		printf("%s ", argv[i]);
	}
	// std_n_rec(50000);
	return argc;
}
