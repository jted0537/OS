CC = gcc
TARGET = testlib
OBJS = main.o bitmap.o debug.o hex_dump.o list.o hash.o

CFLAGS = -Wall
LDFLAGS = -lc

all: $(TARGET)

$(TARGET) : $(OBJS)
		$(CC) $(LDFLAGS) -o $@ $^

.c.o : 
		$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)
