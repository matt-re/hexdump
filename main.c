#include <stdio.h>
#include <stdlib.h>

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
	int result = EXIT_FAILURE;

	FILE *file = NULL;
	const char *filename = NULL;
	if (argc > 1) {
		file = fopen(argv[1], "rb");
		filename = argv[1];
	} else {
		file = stdin;
		filename = "stdin";
	}
	if (!file) {
		perror(filename);
		goto cleanup;
	}

	unsigned char buf[4096];
	size_t offset = 0;
	size_t nread;
	while ((nread = fread(buf, 1, sizeof(buf), file)) > 0) {
		hexdump(buf, nread, offset, print_line);
		offset += nread;
	}
	if (ferror(file)) {
		perror(filename);
		goto cleanup;
	}
	result = EXIT_SUCCESS;

cleanup:
	if (file && file != stdin) {
		fclose(file);
	}

	return result;
}
