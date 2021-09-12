#Compiler
cc = gcc

#Compiler Flags
CFLAGS = -Wall

#Targets 
TARGET = zing zing2 clean

all: $(TARGET)

zing: main.o zing.o 
	$(CC) $(CFLAGS) main.o zing.o -o zing

zing2: main.o zing2.o
	$(CC) $(CFLAGS) main.o zing2.o -o zing2

main.o: main.c
	$(CC) $(CFLAGS) main.c -c 

zing2.o: zing2.c
	$(CC) $(CFLAGS) zing2.c -c

zing.o:
	touch  zing.o

clean : 
	rm main.o zing2.o
