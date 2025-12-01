CC	:= clang
CFLAGS	:= -std=c99		\
	   -O2			\
	   -fstrict-aliasing	\
	   -pedantic		\
	   -Wall		\
	   -Werror		\
	   -Wextra		\
	   -Wshadow		\
	   -Wconversion		\
	   -Wstrict-aliasing
LDFLAGS	:=

TARGET	:= hexdump
OBJS	:= main.o

.PHONY: all clean run

all: $(TARGET)

%.o: %.c hexdump.h
	$(CC) -c $(CFLAGS) $<

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	@rm -f $(TARGET) $(OBJS)

run: $(TARGET)
	./$(TARGET) main.c
