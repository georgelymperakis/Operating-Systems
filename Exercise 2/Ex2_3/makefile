
#Compiler
CC  = gcc

#Compiler Flags
CFLAGS = -Wall

#Targets
TARGET = ex21 clean

all: $(TARGET)

ex21: ex21.o proc-common.o tree.o
	$(CC) $(CFLAGS) ex21.o proc-common.o tree.o -o ex21

ex21.o: ex21.c
	$(CC) $(CFLAGS) ex21.c -c

proc-common.o: proc-common.c
	$(CC) $(CFLAGS) proc-common.c -c

tree.o: tree.c
	$(CC) $(CFLAGS) tree.c -c

clean:
	rm ex21.o proc-common.o tree.o




