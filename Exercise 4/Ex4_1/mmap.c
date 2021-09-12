/*
 * mmap.c
 *
 * Examining the virtual memory of processes.
 *
 * Operating Systems course, CSLab, ECE, NTUA
 *
 */

//----------------------
//Extra Libraries

#include <inttypes.h>
#include <sys/stat.h>



//----------------------

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <signal.h>
#include <sys/wait.h>

#include "help.h"

#define RED     "\033[31m"
#define RESET   "\033[0m"


char *heap_private_buf;
char *heap_shared_buf;

char *file_shared_buf;

uint64_t buffer_size;

/*
 * Child process' entry point.
 */
void child(void)
{
//	uint64_t pa;

	/*
	 * Step 7 - Child
	 */
	if (0 != raise(SIGSTOP))
		die("raise(SIGSTOP)");
	/*
	 * TODO: Write your code here to complete child's part of Step 7.
	 */
	printf("\n\nChild Process:\n");
	show_maps();
//---------------------------------------------------------------------------------------------------------------------

	/*
	 * Step 8 - Child
	 */
	if (0 != raise(SIGSTOP))
		die("raise(SIGSTOP)");

	uint64_t vc = (uint64_t) heap_private_buf;
	/*
	 * TODO: Write your code here to complete child's part of Step 8.
	 */
	//fill buffer with 8s

	printf("\n\nChild Process:\n");
	printf("\nInformation for the VA of the buffer:\n");
	show_va_info(vc);
	
	printf("\n\nInformation for the PA of the buffer: %"PRIu64" \n",get_physical_address(vc));	

//---------------------------------------------------------------------------------------------------------------------
	/*
	 * Step 9 - Child
	 */
	if (0 != raise(SIGSTOP))
		die("raise(SIGSTOP)");
	/*
	 * TODO: Write your code here to complete child's part of Step 9.
	 */

	unsigned i;
	int * arr = (void *) heap_private_buf;
	long page_size = get_page_size();
	for (i = 0; i < (page_size/sizeof(int)); ++i){
        	arr[i]=17;
	}

	printf("\n\nChild Process:\n");
	printf("\nInformation for the VA of the buffer:\n");
	show_va_info(vc);
	
	printf("\n\nInformation for the PA of the buffer: %"PRIu64" \n",get_physical_address(vc));	


	/*
	 * Step 10 - Child
	 */
		if (0 != raise(SIGSTOP))
		die("raise(SIGSTOP)");

	/*
	 * TODO: Write your code here to complete child's part of Step 10.
	 */

		uint64_t vcs = (uint64_t) heap_shared_buf;
		unsigned j;
		int * arrshared = (void *) heap_shared_buf;
		for (j = 0; j < (page_size/sizeof(int)); ++j){
        	arrshared[j]=8;
		}

	printf("\n\nChild Process:\n");
	printf("\nInformation for the VA of the buffer:\n");
	show_va_info(vcs);
	
	printf("\n\nInformation for the PA of the buffer: %"PRIu64" \n",get_physical_address(vcs));	



	/*
	 * Step 11 - Child
	 */
	if (0 != raise(SIGSTOP))
		die("raise(SIGSTOP)");
	/*
	 * TODO: Write your code here to complete child's part of Step 11.
	 */
	mprotect(heap_shared_buf,page_size,PROT_READ);
	printf("\n\nChild Process:\n");
	printf("\nInformation for the VA of the buffer:\n");
	show_va_info(vcs);
	
	printf("\n\nInformation for the PA of the buffer: %"PRIu64" \n",get_physical_address(vcs));	
	//show_maps();
	/*
	 * Step 12 - Child
	 */
	/*
	 * TODO: Write your code here to complete child's part of Step 12.
	 */
}

/*
 * Parent process' entry point.
 */
void parent(pid_t child_pid)
{
//	uint64_t pa;
	int status;

	/* Wait for the child to raise its first SIGSTOP. */
	if (-1 == waitpid(child_pid, &status, WUNTRACED))
		die("waitpid");
//------------------------------------------------------------------------------
	/*
	 * Step 7: Print parent's and child's maps. What do you see?
	 * Step 7 - Parent
	 */
	printf(RED "\nStep 7: Print parent's and child's map.\n" RESET);
	press_enter();


	/*
	 * TODO: Write your code here to complete parent's part of Step 7.
	 */
	printf("Father Process:\n");
	show_maps();

	if (-1 == kill(child_pid, SIGCONT))
		die("kill");
	if (-1 == waitpid(child_pid, &status, WUNTRACED))
		die("waitpid");

//------------------------------------------------------------------------------	
	/*
	 * Step 8: Get the physical memory address for heap_private_buf.
	 * Step 8 - Parent
	 */
	uint64_t vp = (uint64_t) heap_private_buf;
	printf(RED "\nStep 8: Find the physical address of the private heap "
		"buffer (main) for both the parent and the child.\n" RESET);
	press_enter();

	/*
	 * TODO: Write your code here to complete parent's part of Step 8.
	 */
	printf("Father Process:\n");
	printf("\nInformation for the VA of the buffer:\n");
	show_va_info(vp);
	printf("\n\nInformation for the PA of the buffer: %"PRIu64" \n",get_physical_address(vp));	


	if (-1 == kill(child_pid, SIGCONT))
		die("kill");
	if (-1 == waitpid(child_pid, &status, WUNTRACED))
		die("waitpid");
//------------------------------------------------------------------------------------

	/*
	 * Step 9: Write to heap_private_buf. What happened?
	 * Step 9 - Parent
	 */
	printf(RED "\nStep 9: Write to the private buffer from the child and "
		"repeat step 8. What happened?\n" RESET);
	press_enter();

	/*
	 * TODO: Write your code here to complete parent's part of Step 9.
	 */



	printf("Father Process:\n");
	printf("\nInformation for the VA of the buffer:\n");
	show_va_info(vp);
	printf("\n\nInformation for the PA of the buffer: %"PRIu64" \n",get_physical_address(vp));	


	if (-1 == kill(child_pid, SIGCONT))
		die("kill");
	if (-1 == waitpid(child_pid, &status, WUNTRACED))
		die("waitpid");


	/*
	 * Step 10: Get the physical memory address for heap_shared_buf.
	 * Step 10 - Parent
	 */
	printf(RED "\nStep 10: Write to the shared heap buffer (main) from "
		"child and get the physical address for both the parent and "
		"the child. What happened?\n" RESET);
	press_enter();

	/*
	 * TODO: Write your code here to complete parent's part of Step 10.
	 */

	uint64_t vps = (uint64_t) heap_shared_buf;
	printf("Father Process:\n");
	printf("\nInformation for the VA of the buffer:\n");
	show_va_info(vps);
	printf("\n\nInformation for the PA of the buffer: %"PRIu64" \n",get_physical_address(vps));	

	// long page_size = get_page_size();
	// uint64_t vcs = (uint64_t) buff;
	// unsigned j;
	// int * arrshared = (void *) buff;
	// for (j = 0; j < (sizeof(arrshared)/sizeof(int)); ++j){
    // 	arrshared[j]=8;
	// }

	// printf("Father Process after write:\n");//
	// printf("\nInformation for the VA of the buffer:\n");//
	// show_va_info(vps);///
	// printf("\n\nInformation for the PA of the buffer: %"PRIu64" \n",get_physical_address(vps));	//
	if (-1 == kill(child_pid, SIGCONT))
		die("kill");
	if (-1 == waitpid(child_pid, &status, WUNTRACED))
		die("waitpid");


	/*
	 * Step 11: Disable writing on the shared buffer for the child
	 * (hint: mprotect(2)).
	 * Step 11 - Parent
	 */
	printf(RED "\nStep 11: Disable writing on the shared buffer for the "
		"child. Verify through the maps for the parent and the "
		"child.\n" RESET);
	press_enter();

	/*
	 * TODO: Write your code here to complete parent's part of Step 11.
	 */


	uint64_t vps2 = (uint64_t) heap_shared_buf;
	printf("Father Process:\n");
	printf("\nInformation for the VA of the buffer:\n");
	show_va_info(vps2);
	printf("\n\nInformation for the PA of the buffer: %"PRIu64" \n",get_physical_address(vps2));	
	//show_maps();

	if (-1 == kill(child_pid, SIGCONT))
		die("kill");
	if (-1 == waitpid(child_pid, &status, 0))
		die("waitpid");

 
	/*
	 * Step 12: Free all buffers for parent and child.
	 * Step 12 - Parent
	 */
		if (munmap(heap_shared_buf, sizeof(heap_shared_buf)) == -1) {
			perror("shared buffer munmap failed");
			exit(1);
		}
		if (munmap(heap_private_buf, sizeof(heap_private_buf)) == -1) {
			perror("private buffer munmap failed");
			exit(1);
		}

	/*
	 * TODO: Write your code here to complete parent's part of Step 12.
	 */
}

int main(void)
{
	pid_t mypid, p;
	int fd = -1;
//	uint64_t pa;


	mypid = getpid();
	buffer_size = 1 * get_page_size();

	printf("\nProcess PID: %d \n",mypid);
//-------------------------------------------------------------------------------------------------------------	
	/*
	 * Step 1: Print the virtual address space layout of this process.
	 */
	printf(RED "\nStep 1: Print the virtual address space map of this "
		"process [%d].\n" RESET, mypid);
	press_enter();

	show_maps();

//-------------------------------------------------------------------------------------------------------------
	/*
	 * Step 2: Use mmap to allocate a buffer of 1 page and print the map
	 * again. Store buffer in heap_private_buf.
	 */
	printf(RED "\nStep 2: Use mmap(2) to allocate a private buffer of "
		"size equal to 1 page and print the VM map again.\n" RESET);
	press_enter();
	

	long page_size = get_page_size();
	printf("Page Size : %ld Bytes or %ld KBytes\n",page_size,page_size/1024);
	


	if((heap_private_buf = mmap(NULL,page_size,PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS,-1,0))==MAP_FAILED){//changed -1 to fd
	 	printf("Mapping Failed !! \n");
		exit(-1);
	}
        uint64_t va = (uint64_t) heap_private_buf;

        printf("\nInformation for the VA of the buffer:\n");
        show_va_info(va);


	show_maps();

//----------------------------------------------------------------------------------------------------------------------
	/*
	 * Step 3: Find the physical address of the first page of your buffer
	 * in main memory. What do you see?
	 */
	printf(RED "\nStep 3: Find and print the physical address of the "
		"buffer in main memory. What do you see?\n" RESET);
	press_enter();
	


	printf("\nInformation for the VA of the buffer:\n");
	show_va_info(va);
	
	printf("\n\nInformation for the PA of the buffer: %"PRIu64" \n",get_physical_address(va));	

//---------------------------------------------------------------------------------------------------------------------
	/*
	 * Step 4: Write zeros to the buffer and repeat Step 3.
	 */
	printf(RED "\nStep 4: Initialize your buffer with zeros and repeat "
		"Step 3. What happened?\n" RESET);
	press_enter();

	unsigned i;
	int * arr = (void *) heap_private_buf;

	for (i = 0; i < (page_size/sizeof(int)); ++i){
        	arr[i]=0;
	}
/*
	for(i=0; i <(page_size/sizeof(int)); ++i){
	       printf("Buffer[%d] = %d \n",i,arr[i]);
	}
*/

        printf("\nInformation for the VA of the buffer:\n");
        show_va_info(va);

        printf("\n\nInformation for the PA of the buffer: %"PRIu64" \n",get_physical_address(va));
	show_maps();

//----------------------------------------------------------------------------------------------------------------------------
	/*
	 * Step 5: Use mmap(2) to map file.txt (memory-mapped files) and print
	 * its content. Use file_shared_buf.
	 */
	printf(RED "\nStep 5: Use mmap(2) to read and print file.txt. Print "
		"the new mapping information that has been created.\n" RESET);
	press_enter();

	char * filename ="file.txt";//"/home/oslab/oslaba69/ALL/G/mmap/file.txt";

	if((fd = open(filename,O_RDONLY)) == -1){
		perror("OPEN");
		exit(-1);
	}


	struct stat st;

	if(fstat(fd,&st)==-1){
	        perror("Get File Size");
		exit(-1);
	}

	unsigned length = st.st_size;


	if(length  == 0){
		printf("\nEmpty File\n");
	}

	char * text;

	if((text = mmap(NULL,length,PROT_READ,MAP_SHARED,fd,0))==MAP_FAILED){
		printf("Mapping Failed!\n");
		close(fd);
		exit(-1);
	}


	for(i = 0; i<length; ++i){
		printf("Text[%d] = %c\n",i,text[i]);	
	}

	//close(fd);

	//show_maps();

        va = (uint64_t) text;

        printf("\nInformation for the VA of the buffer:\n");
        show_va_info(va);

        printf("\n\nInformation for the PA of the buffer: %"PRIu64" \n",get_physical_address(va));
	show_maps();

//-----------------------------------------------------------------------------------------------------------------------------
	/*
	 * Step 6: Use mmap(2) to allocate a shared buffer of 1 page. Use
	 * heap_shared_buf.
	 */
	printf(RED "\nStep 6: Use mmap(2) to allocate a shared buffer of size "
		"equal to 1 page. Initialize the buffer and print the new "
		"mapping information that has been created.\n" RESET);
	press_enter();



	// char *buffer;
	// if((buffer = mmap(NULL,page_size,PROT_READ | PROT_WRITE | PROT_EXEC,MAP_SHARED,0,0))==MAP_FAILED){
	// 	printf("Mapping Failed!\n");
	// }
	
	// //show_maps();

    //     va = (uint64_t) buffer;

    //     printf("\nInformation for the VA of the buffer:\n");
    //     show_va_info(va);

    //     printf("\n\nInformation for the PA of the buffer: %"PRIu64" \n",get_physical_address(va));
	//long page_size = get_page_size();
	printf("Page Size : %ld Bytes or %ld KBytes\n",page_size,page_size/1024);
	

	

	if((heap_shared_buf = mmap(NULL,page_size,PROT_READ | PROT_WRITE,MAP_SHARED| MAP_ANONYMOUS,-1,0))==MAP_FAILED){
	 	printf("Mapping Failed !! \n");
		exit(-1);
	}
	uint64_t v = (uint64_t) heap_shared_buf;

	printf("\nInformation for the VA of the buffer:\n");
	show_va_info(v);
	
	printf("\n\nInformation for the PA of the buffer: %"PRIu64" \n",get_physical_address(v));	

	show_maps();

	p = fork();
	if (p < 0)
		die("fork");
	if (p == 0) {
		child();
		return 0;
	}

	parent(p);
	if (-1 == close(fd))//fd
		perror("close");
	return 0;
}

