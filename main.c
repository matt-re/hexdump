#include <stdio.h>

#define HEXDUMP_IMPLEMENTATION
#include "hexdump.h"

static void
print_line(const char *line, size_t len)
{
	printf("%.*s\n", (int)len, line);
}

int
main(int argc, char *argv[])
{
	int result = 1;

	FILE *file = argc > 1 ? fopen(argv[1], "rb") : stdin;
	if (!file) {
		perror("File read error");
		goto done;
	}

	unsigned char buf[4096];
	size_t offset = 0;
	for (;;) {
		size_t nread = fread(buf, 1, sizeof buf, file);
		if (nread == 0) {
			if (ferror(file)) {
				perror("File read error");
				goto done;
			}
			break;
		}
		hexdump(buf, nread, offset, print_line);
		offset += nread;
	}
	result = 0;

done:
	if (file && file != stdin) {
		fclose(file);
	}

	return result;
}
