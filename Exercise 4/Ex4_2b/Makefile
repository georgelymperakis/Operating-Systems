.PHONY: all clean

all: mmap

CC = gcc
CFLAGS = -g -Wall -Wextra -O2
SHELL= /bin/bash

mmap: mmap.o help.o
	$(CC) $(CFLAGS) $^ -o $@

%.s: %.c
	$(CC) $(CFLAGS) -S -fverbose-asm $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.i: %.c
	gcc -Wall -E $< | indent -kr > $@

clean: 
	rm -f *.o mmap
