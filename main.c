#include <stdio.h>

#define HEXDUMP_IMPLEMENTATION
#include "hexdump.h"

static void
print(const char *line)
{
	printf("%s\n", line);
}

int
main(int argc, char *argv[])
{
	FILE *file = NULL;
	if (argc > 1) {
		file = fopen(argv[1], "rb");
		if (!file) {
			return 1;
		}
	}
	char buf[512];
	uintptr_t whence = 0;
	for (;;) {
		size_t n = fread(buf, sizeof *buf, sizeof buf, file ? file : stdin);
		if (n == 0) {
			break;
		}
		hexdump(buf, n, whence, print);
		whence += n;
	}
	if (file) {
		fclose(file);
	}
	return 0;
}

