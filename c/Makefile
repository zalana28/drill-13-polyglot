CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2

test: test.c ringbuffer.c lru.c ringbuffer.h lru.h
	$(CC) $(CFLAGS) -o test test.c ringbuffer.c lru.c

check: test
	./test

clean:
	rm -f test

.PHONY: check clean
