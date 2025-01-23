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
	FILE *file = argc > 1 ? fopen(argv[1], "rb") : stdin;
	if (!file) {
		return 1;
	}
	char buf[512];
	uintptr_t whence = 0;
	for (;;) {
		size_t nread = fread(buf, sizeof *buf, sizeof buf, file);
		if (nread == 0) {
			break;
		}
		hexdump(buf, nread, whence, print);
		whence += nread;
	}
	if (file != stdin) {
		fclose(file);
	}
	return 0;
}

