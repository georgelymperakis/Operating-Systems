	
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "tree.h"
#include "proc-common.h"

//Inter-Process Communication

typedef struct tree_node* tree;

#define SLEEP_TIME 0.5

void signal_handler_cont(int sig){

        printf("Received Signal to continue %d ,PID:  %d  \n",sig,getpid() );
	sleep(SLEEP_TIME);
}




void fork_procs(tree root)
{
	pid_t * pid_array;
	pid_t p;
	int  status;
	unsigned int i ;

//	printf("PID = %ld, name %s, starting...\n",(long)getpid(), root->name);
	change_pname(root->name);

	unsigned int Num_Chil = root -> nr_children;

	if(Num_Chil) {
		pid_array = malloc(sizeof(pid_t) * Num_Chil);
		if (pid_array == NULL){
			perror("Problem with Malloc");
			exit(-1);
		}
	}


	for(i = 0; i < Num_Chil; ++i){

		pid_array[i] = fork();

		if (pid_array[i] < 0){
        	 perror("child process was unsuccessful");
                 exit(-1);
                }

		if (pid_array[i]== 0){
		  sleep(SLEEP_TIME);
		  change_pname((root -> children + i) -> name);
		  printf("PID=%ld ,name = %s is created and going to sleep for now\n",(long) getpid(), (root->children + i)->name);
	 	  
		  signal(SIGCONT,signal_handler_cont);
		  fork_procs(root->children + i);

           }


	}

	if (Num_Chil != 0){
	 wait_for_ready_children(Num_Chil);
	}

	raise(SIGSTOP);  //equiavelent to kill(get_pid(),SIGSTOP)
	sleep(2*SLEEP_TIME);
	
        printf("PID : %d with name %s , is awake\n",getpid(),root->name);

	for(i = 0; i < Num_Chil ; ++i){
		kill(pid_array[i],SIGCONT);

        	p = wait(&status);
        	explain_wait_status(p, status);

	}

  	printf("PID : %d with name %s , is Exiting\n",getpid(),root->name);
	exit(0);
}	

/*
 * The initial process forks the root of the process tree,
 * waits for the process tree to be completely created,
 * then takes a photo of it using show_pstree().
 *
 * How to wait for the process tree to be ready?
 * In ask2-{fork, tree}:
 *      wait for a few seconds, hope for the best.
 * In ask2-signals:
 *      use wait_for_ready_children() to wait until
 *      the first process raises SIGSTOP.
 */

int main(int argc, char *argv[])
{
	pid_t pid;
	int status;
	struct tree_node *root;

	if (argc < 2){
		fprintf(stderr, "Usage: %s <tree_file>\n", argv[0]);
		exit(1);
	}

	/* Read tree into memory */
	root = get_tree_from_file(argv[1]);

	/* Fork root of process tree */
	pid = fork();
	if (pid < 0) {
		perror("main: fork");
		exit(1);
	}
	if (pid == 0) {
		/* Child */
		fork_procs(root);
		exit(1);
	}

	/*
	 * Father
	 */ 
	/* for ask2-signals */
	wait_for_ready_children(1);

	/* for ask2-{fork, tree} */
	/* sleep(SLEEP_TREE_SEC); */

	/* Print the process tree root at pid */
	show_pstree(pid);

	/* for ask2-signals */
	kill(pid, SIGCONT);

	/* Wait for the root of the process tree to terminate */
	wait(&status);
	explain_wait_status(pid, status);

	return 0;
}


