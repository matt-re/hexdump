#ifndef INCLUDE_HEXDUMP_H
#define INCLUDE_HEXDUMP_H

#include <stddef.h>
#include <stdint.h>

typedef void (*hexdump_callback)(const char *line, size_t len, int lastline);

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
	/* bytes per row/line */
	unsigned int bpr = 16;
	if (bpr == 0) {
		bpr = 16;
	} else if (bpr > 256) {
		bpr = 256;
	}
	/* max line length 916, when bpr == 256
	 * index  size  description
	 * ------------------------
	 * 0      16    address in bytes in hex (64-bit pointer)
	 * 16     2     separator between address and data
	 * 18     512   bytes per line printed in hex
	 * 530    128   char separator between each 2 bytes
	 *              including after the last one
	 * 658    1     char separator between hex and ascii
	 * 659    256   bytes per line printed as ascii
	 * 915    1     nul terminator
	 */
	char line[(sizeof(void *) * 2) + 2 + (256 * 2) + (256 / 2) + 1 + 256 + 1];
	const size_t addrlen = sizeof(void *) * 2;
	const size_t dataoff = addrlen + 2;
	const size_t datalen = 2 * bpr + bpr / 2;
	const size_t textoff = dataoff + datalen + 1;
	const size_t linelen = textoff + bpr + 1;
	for (char *ch = line; ch < (line + linelen - 2); ch++) {
		*ch = ' ';
	}
	line[linelen - 1] = '\0';
	const char hex[] = "0123456789abcdef";
	const int zeroaddr = 1;
	if (whence == 0) {
		whence = zeroaddr ? 0 : (uintptr_t)cur;
	}
	while (end > cur) {
		char *addr = line;
		char *data = line + dataoff;
		char *text = line + textoff;
		for (size_t i = sizeof(void *) * 8; i > 0; i -= 4) {
			*addr++ = hex[(whence >> (i - 4)) & 15];
		}
		*addr = ':';
		ptrdiff_t len = (end - cur) < bpr ? (end - cur) : bpr;
		ptrdiff_t i = 0;
		for (; i < len; i++) {
			char ch = cur[i];
			*data++ = hex[(ch >> 4) & 15];
			*data++ = hex[(ch >> 0) & 15];
			*text++ = ch > 31 && ch < 127 ? ch : '.';
			/* jump over separator between every two bytes */
			data += i % 2;
		}
		for (; i < bpr; i++) {
			*data++ = ' ';
			*data++ = ' ';
			*text++ = ' ';
			data += i % 2;
		}
		whence += bpr;
		cur += bpr;
		if (cb) {
			cb(line, linelen, !(end > cur));
		}
	}
}

#endif /* HEXDUMP_IMPLEMENTATION */

