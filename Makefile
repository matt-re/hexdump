CC	= clang
CFLAGS	= -std=c99		\
	  -O2			\
	  -fstrict-aliasing	\
	  -pedantic		\
	  -Wall			\
	  -Werror		\
	  -Wextra		\
	  -Wshadow		\
	  -Wconversion		\
	  -Wstrict-aliasing
LDFLAGS	= -Wall -pedantic

all: hexdump

%.o: %.c hexdump.h
	$(CC) -c $(CFLAGS) $<

hexdump: main.o
	$(CC) -o $@ $(LDFLAGS) $^

.PHONEY: clean
.PHONEY: run

clean:
	@rm -f hexdump *.o

run: hexdump
	./hexdump

