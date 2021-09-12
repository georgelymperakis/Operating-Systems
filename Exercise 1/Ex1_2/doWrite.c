
#include "Write.h"
void doWrite(int fd, const char *buff, int len){
	size_t idx = 0;
	ssize_t wcnt;
	do{

		wcnt = write(fd,buff+idx, len - idx);
		if(wcnt == -1){
			perror("ERROR WRITE\n");
			exit(1);
		}
		idx += wcnt;
	}while(idx < len);

}
