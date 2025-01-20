#include <stdio.h>

#define HEXDUMP_IMPLEMENTATION
#include "hexdump.h"

static void
print(const char *line, size_t len)
{
	(void)len;
	printf("%s\n", line);
}

int
main(int argc, char *argv[])
{
	FILE *file = fopen(argc > 1 ? argv[1] : "hexdump.h", "rb");
	if (!file) return 1;
	char buf[512];
	uintptr_t whence = 0;
	for (;;) {
		size_t n = fread(buf, sizeof *buf, sizeof buf, file);
		if (n == 0) break;
		hexdump(buf, n, whence, print);
		whence += n;
	}
	fclose(file);
	return 0;
}

