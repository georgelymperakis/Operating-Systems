#include "stdio.h"
#include <unistd.h>

void zing(void){
	char* username = getlogin();
	printf("Hello, %s !! I am the second version\n",username);
}
