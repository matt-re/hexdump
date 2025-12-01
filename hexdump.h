#ifndef INCLUDE_HEXDUMP_H
#define INCLUDE_HEXDUMP_H

#include <stddef.h>

typedef void (*hexdump_callback)(const char *line, size_t len);

#ifdef __cplusplus
extern "C" {
#endif
void hexdump(void *ptr, size_t size, size_t offset, hexdump_callback callback);
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_HEXDUMP_H */

#ifdef HEXDUMP_IMPLEMENTATION

void
hexdump(void *ptr, size_t size, size_t offset, hexdump_callback callback)
{
	static const char hex[] = "0123456789abcdef";
	const unsigned char *cur = ptr;
	const unsigned char * const end = cur + size;

	/* offset size
	 * ------------------------
	 * 0      8    address offset in hex
	 * 8      1    " " separator
	 * 9      40   16 bytes of data in hex and 8 " " separators
	 * 49     1    " " separator
	 * 50     16   16 bytes of data in ASCII
	 * 66     1    nul terminator
	 */
	char line[67];

	line[8] = ' ';
	line[13] = line[18] = line[23] = line[28] = ' ';
	line[33] = line[38] = line[43] = line[48] = ' ';
	line[49] = ' ';

	while (cur < end) {
		char *addr = line;
		char *data = line + 9;
		char *text = line + 50;

		for (int i = 7; i >= 0; --i) {
			*addr++ = hex[(offset >> (i * 4)) & 15];
		}

		const ptrdiff_t remaining_bytes = end - cur;
		const ptrdiff_t line_bytes = remaining_bytes < 16 ? remaining_bytes : 16;
		for (ptrdiff_t i = 0; i < line_bytes; ++i) {
			const unsigned char byte = cur[i];
			*data++ = hex[byte >> 4];
			*data++ = hex[byte & 15];
			data += i & 1;
			*text++ = (byte >= 32 && byte < 127) ? (char)byte : '.';
		}
		*text = '\0';

		for (ptrdiff_t i = line_bytes; i < 16; ++i) {
			*data++ = ' ';
			*data++ = ' ';
			data += i & 1;
		}

		if (callback) {
			callback(line, (size_t)(text - line));
		}

		offset += 16;
		cur += 16;
	}
}

#endif /* HEXDUMP_IMPLEMENTATION */
