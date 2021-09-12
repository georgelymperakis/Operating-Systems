#include "Write.h"

int main (int argc,char **argv){
	char *outfile , *infile1, *infile2;
	if (argc < 3 || argc > 4){
		printf("./fconc infile1 infile2 [outfile (default:fconc.out)]\n");
		return 1;
	}
	else if(argc == 4){
		outfile =  argv[3];
	}
	else outfile =  "fconc.out";
	infile1 = argv[1];
	infile2 = argv[2];
	int fd;
	fd = open(outfile,O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if (fd == -1){
		perror("ERROR OPEN OUTFILE\n");
		exit(1);
	}

	write_file(fd,infile1);
	write_file(fd,infile2);

	close(fd);
	return 0;
}
