#include <stdio.h>
#include <string.h>

#ifndef _WIN32
#include <unistd.h>
#endif /* _WIN32 */

#define HEXDUMP_IMPLEMENTATION
#include "hexdump.h"

#ifdef _WIN32
#define STDOUT_FILENO ((uintptr_t)stdout)

static int
os_write(uintptr_t fd, const void *buf, size_t size)
{
	/* TODO replace with Win32 Write API calls */
	if (!fd) {
		return 0;
	}
	size_t nitems = fwrite(buf, 1, size, (FILE *)fd);
	return nitems == size;
}
#else /* _WIN32 */
static int
os_write(uintptr_t fd, const void *buf, size_t size)
{
	int filedes = (int)fd;
	if (!filedes) {
		return 0;
	}
	int result = 1;
	while (size) {
		size_t nwrite = (size_t)write(filedes, buf, size);
		if (nwrite == (size_t)-1) {
			result = 0;
			break;
		}
		if (nwrite == size) {
			break;
		}
		buf = (char *)buf + nwrite;
		size -= nwrite;
	}
	return result;
}
#endif /* _WIN32 */

/*
 * Buffered output from the blog post
 * "Let's implement buffered, formatted output" by Chris Wellons.
 * https://nullprogram.com/blog/2023/02/13/
 */
struct buffer
{
	char *buf;
	size_t cap;
	size_t len;
	uintptr_t fd;
	int err;
};

static void
flush(struct buffer *buf)
{
	if (!buf->err && buf->len) {
		buf->err |= !os_write(buf->fd, buf->buf, buf->len);
		buf->len = 0;
	}
}

static void
append(struct buffer *buf, const char *src, size_t len)
{
	const char *end = src + len;
	while (!buf->err && src < end) {
		size_t left = (size_t)(end - src);
		size_t avail = buf->cap - buf->len;
		size_t amount = avail < left ? avail : left;
		memcpy(buf->buf + buf->len, src, amount);
		src += amount;
		buf->len += amount;
		if (amount < left) {
			flush(buf);
		}
	}
}

static void
append_char(struct buffer *buf, char c)
{
	append(buf, &c, 1);
}

static void
print(const char *line, size_t len, void *data)
{
	struct buffer *buf = (struct buffer *)data;
	append(buf, line, len);
	append_char(buf, '\n');
}

int
main(int argc, char *argv[])
{
	FILE *file = argc > 1 ? fopen(argv[1], "rb") : stdin;
	if (!file) {
		return 1;
	}
	char buf[512];
	unsigned int whence = 0;
	char outbuf[1024];
	struct buffer output = {
		.buf = outbuf,
		.cap = sizeof outbuf,
		.len = 0,
		.err = 0,
		.fd = STDOUT_FILENO
	};
	for (;;) {
		size_t nread = fread(buf, sizeof *buf, sizeof buf, file);
		if (nread == 0) {
			break;
		}
		hexdump(buf, nread, whence, print, &output);
		whence += nread;
	}
	if (file != stdin) {
		fclose(file);
	}
	flush(&output);
	return output.err;
}

