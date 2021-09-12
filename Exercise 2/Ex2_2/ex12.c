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

#define SLEEP_LEAF 15   
#define SLEEP_TREE_SEC 6

void print_children(struct tree_node *p){ //Print the children names of  a node
 
    printf("All my Children are { ");
    unsigned int i ;
 
    for(i = 0; i < p->nr_children; ++i){
           printf(" %s  ",(p->children + i) -> name);
    
    if (i< p->nr_children -1){
         printf(","); 
    }
  }
       printf("} \n");
}

void Make_Tree_Proc(struct tree_node *root){
    //declaretions 
    pid_t p;
    int i,status ;

    change_pname(root -> name);  //name the process 
 //   printf("I am %s and I am alive !!\n",root->name);

    for(i = 0; i < root -> nr_children; ++i){ //visit every child of node

     //Say who you are and how many kids do you have 
//     printf("I am %s and have [ %d ] children to make , i.e. ",root -> name,root->nr_children - i);
    // print_children(root);

     // call fork 
     p = fork();
    
     //Check if there is any problem with fork implementation
     if (p < 0) {
        perror("fork");
	  exit(-1);
     }
    //Code for the child process
     if ( p == 0 ) {
         
	//change_pname((root -> children + i) -> name);
        printf("I am %s and I am alive ! \n",(root->children + i)->name);
         if ((root -> children + i)->nr_children == 0){  //Are you a leaf?
            // printf("I am the %s Process and I am a Leaf\n", (root-> children + i)->name);  //say that you are a leaf!
		change_pname((root->children+i)->name);
            

            // printf("Process %s : I am alive !\n",(root-> children+ i)->name); //Say when you are alive
             sleep(SLEEP_LEAF);                                           //rest
             printf("I am %s and I am dying !\n",(root-> children + i)->name); //Say when you are done
             exit(1); 
         }

         else { //Then you are a median node and call recursive the function for your next children
           // printf("I am the %s Process and I am a Median node \n",(root->children+i)->name);
	    Make_Tree_Proc(root -> children + i); 
        }
     }
} 
  
     for(i=0; i < root -> nr_children; ++i){
        // printf("Loop: %s hi \n",root->name);
       p = wait(&status);
       explain_wait_status(p,status);
  //     printf("Hi %s \n",root->name); 
      } 
     printf("I am %s and I am dying !\n",root->name);
     exit(1); //Exit the middle node
}

int main(int argc, char *argv[]) {
	struct tree_node * root;
        int status;

	if (argc != 2){ //Check if the argument is not there
		fprintf(stderr, "Usage: %s<input_tree_file>\n\n", argv[0]);
		exit(1);
	}

	printf("\n");
	printf("The fork Tree is the above:\n\n");
	root = get_tree_from_file(argv[1]); 
	print_tree(root);

	printf("\n\n ---------------  Process Tree Generation  ----------------- \n\n");

	pid_t pid = fork(); //generate the first process in order to start

	if (pid < 0) //Check if there is a problem
	{
		perror("fork");
		exit(-1);
	}
        else if ( pid == 0) { //What to do the process child
           printf("I am %s and I am alive ! \n",root->name);
	   Make_Tree_Proc(root);
           exit(1);
        }

    //code for the first process
    
    sleep(SLEEP_TREE_SEC);
    
    show_pstree(pid); //print process tree
	
    pid = wait(& status); //wait father
    explain_wait_status(pid, status); //

	
	return 0;
}
