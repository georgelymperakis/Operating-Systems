#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "proc-common.h"

#define SLEEP_PROC_SEC  10
#define SLEEP_TREE_SEC  3

/*
 * Create this process tree:
 * A-+-B---D
 *   `-C
 */

void fork_procs(void)
{
	pid_t b,c,d;
	int status;
	// Create B Child
	b = fork();
	if (b < 0) {
		perror("main: fork");
		exit(1);
	}
	if (b == 0) {
		//Create D Child
		d = fork();
		if (d < 0) {//error check
			perror("main: fork");
			exit(1);
		}
		if (d == 0) {//Body of D

			change_pname("D");
			printf("D: Sleeping...\n");
			sleep(SLEEP_PROC_SEC);
			printf("D: Exiting...\n");
			exit(13);
		}
		// Body of B
		change_pname("B");
		sleep(SLEEP_PROC_SEC);
		d = wait(& status);
       		explain_wait_status(d, status);
		printf("B: Exiting...\n");
		exit(19);
	}
	// Create C Child
	c = fork();
	if (c < 0) {
		perror("main: fork");
		exit(1);
	}
	if (c == 0) {
		//Body of C
		change_pname("C");
		printf("C: Sleeping...\n");
		sleep(SLEEP_PROC_SEC);
		printf("C: Exiting...\n");
		exit(17);
	}
	//Body of A
	change_pname("A");
	printf("A: Sleeping...\n");
	sleep(SLEEP_PROC_SEC);
	c = wait(& status);
        explain_wait_status(c, status);
        d = wait(& status);
        explain_wait_status(d, status);
	printf("A: Exiting...\n");
	exit(16);
}


int main(void)
{
	pid_t pid;
	int status;

	/* Fork root of process tree */
	pid = fork();
	if (pid < 0) {
		perror("main: fork");
		exit(1);
	}
	if (pid == 0) {
		/* Child */
		fork_procs();
		exit(1);
	}


	sleep(SLEEP_TREE_SEC);

	/* Print the process tree root at pid */

	show_pstree(pid);
	//show_pstree(getpid());//for question 2

	/* Wait for the root of the process tree to terminate */

	pid = wait(& status);
	explain_wait_status(pid, status);

	return 0;
}
