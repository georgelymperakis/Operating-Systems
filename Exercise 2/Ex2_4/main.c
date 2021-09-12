#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "tree.h"
#include "proc-common.h"




void Make_Tree_Proc(struct tree_node *root,int pipfd[2]){
    //declaretions 
    int pip1[2];
    int pip2[2];
    int input1, input2;
    pid_t p1, p2;

    int i,status ;
    pipe (pip1);
    pipe (pip2);

    change_pname(root -> name);  //name the process 
    int nr_of_children = root->nr_children;

    if (nr_of_children == 2){//middle node
        p1 = fork();
        if (p1 == 0){
            change_pname((root->children) -> name);  //name the process 
            Make_Tree_Proc(root->children, pip1);
        }

        p2 = fork();

        if (p2 == 0){//child process
            change_pname((root->children+1) -> name);  //name the process
            Make_Tree_Proc(root->children+1, pip2);//recursive call
        }
	//read child 1 from pipe1

	if(read(pip1[0], &input1, sizeof(input1))!= sizeof(input1)){
		perror("read from pipe");
		exit(-1);
	}

        close(pip1[0]);//close reading end of pipe1

	//read child 2 from pie2
        if(read(pip2[0], &input2, sizeof(input2))!= sizeof(input2)){
		perror("read from pipe");
		exit(-1);
	}

        close(pip2[0]);//close reading end of pipe2


	int res;
	//calculate result
        if (!strcmp(root->name, "*")){
            res = input1*input2;
        }
        else if (!strcmp(root->name, "+")) {
            res = input1+input2;
        }
        else {//if given anything other than * or +
            printf("UNKNOWN OPERATOR\n"); 
            exit(-1);
        }
	//write result on writing end of father pipe
        if(write(pipfd[1],&res,sizeof(res))!= sizeof(res)){
		perror ("Write to pipe");
		exit (-1);
	}
	close(pipfd[1]); //close writing end of pipe fd

    }
    else {//leaf node
        int val = atoi(root->name);
	//write result on writing end of father pipe
        if(write(pipfd[1], &val, sizeof(val))!= sizeof(val)){
		perror("Write to pipe");
		exit(-1);
	}
	close(pipfd[1]);
        exit(1);//Exit Leaf Node
    }



   for(i=0; i < root -> nr_children; ++i){//wait for all your children to change status
         p1 = wait(&status);
         explain_wait_status(p1,status);
   }

   exit(1); //Exit the middle node
}

int main(int argc, char *argv[]) {
	struct tree_node * root;
        int status;

	if (argc != 2){ //Check if the argument is not there
		fprintf(stderr, "Usage: %s<input_tree_file>\n\n", argv[0]);
		exit(1);
	}
	printf("The fork Tree is the above:\n\n");
	root = get_tree_from_file(argv[1]); 
	print_tree(root);

	printf("\n\n ---------------  Process Tree Generation  ----------------- \n\n");
    int pip[2];
    pipe(pip);
    int res;
	pid_t pid = fork(); //generate the first process in order to start

	if (pid < 0) //Check if there is a problem
	{
		perror("fork");
		exit(-1);
	}
        else if ( pid == 0) { //What to do the process child
            Make_Tree_Proc(root,pip);
            exit(17);
        }

    //code for the first process

    read(pip[0], &res, sizeof(res));
    close(pip[1]);

    //show_pstree(pid); //print process tree

    pid = wait(& status); //wait father
    explain_wait_status(pid, status); //
    printf("\n\nRESULT = %d\n\n",res);

	return 0;
}
