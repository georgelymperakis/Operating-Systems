fconc: fconc.o doWrite.o writefile.o
	gcc -Wall fconc.o doWrite.o writefile.o -o fconc
fconc.o: fconc.c
	gcc -Wall fconc.c -c
doWrite.o: doWrite.c
	gcc -Wall doWrite.c -c

writefile.o: writefile.c
	gcc -Wall writefile.c -c

