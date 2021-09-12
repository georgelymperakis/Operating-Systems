#Compiler
cc = gcc

#Compiler Flags
CFLAGS = -Wall

#Targets
TARGET = ex12 clean

all: $(TARGET)

ex12: ex12.o proc-common.o tree.o
	$(CC) $(CFLAGS) ex12.o proc-common.o tree.o  -o ex12

ex12.o: ex12.c
	$(CC) $(CFLAGS) ex12.c -c

proc-common.o: proc-common.c
	$(CC) $(CFLAGS) proc-common.c -c

tree.o: tree.c
	$(CC) $(CFLAGS) tree.c -c

clean:
	rm ex12.o proc-common.o tree.o  

