#Compiler
cc = gcc

#Compiler Flags
CFLAGS = -Wall

#Targets
TARGET = fork clean

all: $(TARGET)

fork: main.o proc-common.o 
	$(CC) $(CFLAGS) main.o proc-common.o -o fork

main.o: main.c
	$(CC) $(CFLAGS) main.c -c

proc-common.o: proc-common.c
	$(CC) $(CFLAGS) proc-common.c -c
clean:
	rm main.o proc-common.o
