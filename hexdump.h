#ifndef INCLUDE_HEXDUMP_H
#define INCLUDE_HEXDUMP_H

#include <stddef.h>

typedef void (*hexdump_callback)(const char *line, size_t len);

#ifdef __cplusplus
extern "C" {
#endif
void hexdump(const void *ptr, size_t size, unsigned int whence, hexdump_callback cb);
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_HEXDUMP_H */

#ifdef HEXDUMP_IMPLEMENTATION

void
hexdump(const void *ptr, size_t size, unsigned int whence, hexdump_callback cb)
{
	char const * cur = ptr;
	char const * const end = (char *)ptr + size;
	/* index  size  description
	 * ------------------------
	 * 0      8     offset in bytes displayed in hex
	 * 8      2     separator between address and data
	 * 10     32    16 bytes per line printed in hex
	 * 42     8     char separator after each 2 bytes
	 * 50     1     char separator between hex and ascii
	 * 51     16    16 bytes per line printed as ascii
	 * 67     1     nul terminator
	 */
	char line[68];
	/* separator between address and data */
	line[8]  = ':';
	line[9]  = ' ';
	/* separator after each 2 bytes */
	line[14] = ' ';
	line[19] = ' ';
	line[24] = ' ';
	line[29] = ' ';
	line[34] = ' ';
	line[39] = ' ';
	line[44] = ' ';
	line[49] = ' ';
	/* separator between hex and ascii */
	line[50] = ' ';
	const char hex[] = "0123456789abcdef";
	while (end > cur) {
		char *addr = line;
		char *data = line + 10;
		char *text = line + 51;
		for (int i = sizeof(whence) * 7; i >= 0; i -= 4) {
			*addr++ = hex[(whence >> i) & 15];
		}
		ptrdiff_t len = (end - cur) < 16 ? (end - cur) : 16;
		ptrdiff_t i = 0;
		for (; i < len; i++) {
			char ch = cur[i];
			*data++ = hex[(ch >> 4) & 15];
			*data++ = hex[(ch >> 0) & 15];
			*text++ = ch > 31 && ch < 127 ? ch : '.';
			/* jump over separator between every two bytes */
			data += i % 2;
		}
		for (; i < 16; i++) {
			*data++ = ' ';
			*data++ = ' ';
			data += i % 2;
		}
		size_t linelen = (size_t)(text - line);
		line[linelen] = '\0';
		whence += 16;
		cur += 16;
		if (cb) {
			cb(line, linelen);
		}
	}
}

#endif /* HEXDUMP_IMPLEMENTATION */

