#Compiler 
cc = gcc

#Compiler Flags
CFLAGS = -Wall

#Targets
TARGET =  optree clean

all: $(TARGET)
 
optree: main.o proc-common.o tree.o
	$(CC) $(CFLAGS) main.o proc-common.o tree.o -o optree

main.o: main.c
	$(CC) $(CFLAGS) main.c -c

proc-common.o: proc-common.c
	$(CC) $(CFLAGS) proc-common.c -c

tree.o: tree.c
	$(CC) $(CFLAGS) tree.c -c

clean:
	rm main.o proc-common.o tree.o 
