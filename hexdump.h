#ifndef INCLUDE_HEXDUMP_H
#define INCLUDE_HEXDUMP_H

#include <stddef.h>
#include <stdint.h>

typedef void (*hexdump_callback)(const char *line, size_t len);

#ifdef __cplusplus
extern "C" {
#endif
void hexdump(const void *ptr, size_t size, uintptr_t whence, hexdump_callback cb);
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_HEXDUMP_H */

#ifdef HEXDUMP_IMPLEMENTATION

void
hexdump(const void *ptr, size_t size, uintptr_t whence, hexdump_callback cb)
{
	char const * cur = ptr;
	char const * const end = (char *)ptr + size;
	/* index  size  description
	 * ------------------------
	 * 0      16    address in bytes in hex (64-bit pointer)
	 * 16     2     separator between address and data
	 * 18     32    16 bytes per line printed in hex
	 * 50     8     char separator after each 2 bytes
	 * 58     1     char separator between hex and ascii
	 * 59     16    16 bytes per line printed as ascii
	 * 75     1     nul terminator
	 */
	char line[76];
	for (size_t i = 0; i < sizeof(line) - 1; i++) {
		line[i] = ' ';
	}
	line[16] = ':';
	line[sizeof(line) - 1] = '\0';
	const char hex[] = "0123456789abcdef";
	const int zeroaddr = 1;
	if (whence == 0) {
		whence = zeroaddr ? 0 : (uintptr_t)cur;
	}
	while (end > cur) {
		char *addr = line;
		char *data = line + 18;
		char *text = line + 59;
		for (size_t i = sizeof(void *) * 8; i > 0; i -= 4) {
			*addr++ = hex[(whence >> (i - 4)) & 15];
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
			*text++ = ' ';
			data += i % 2;
		}
		whence += 16;
		cur += 16;
		if (cb) {
			cb(line, sizeof(line) - 1);
		}
	}
}

#endif /* HEXDUMP_IMPLEMENTATION */

