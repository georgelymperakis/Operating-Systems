#include"Write.h"

void write_file(int fd, const char *infile){
	int fdi;
        fdi = open (infile,O_RDONLY);
	char buff[1024];
	ssize_t  rcnt;
	for (;;){
		rcnt = read (fdi,buff,sizeof(buff)-1);
		if(rcnt == 0) break;
		if (rcnt == -1){
			perror("ERROR READ\n");
			exit(1);
		}
		buff[rcnt]='\0';
		doWrite (fd,buff,strlen(buff));//random arguments
	}
	close(fdi);
}
