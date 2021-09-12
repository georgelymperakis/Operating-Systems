/*
 * mandel.c
 *
 * A program to draw the Mandelbrot Set on a 256-color xterm.
 *
 */

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <semaphore.h>

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

/*TODO header file for m(un)map*/

#include "mandel-lib.h"

#define MANDEL_MAX_ITERATION 100000

/***************************
 * Compile-time parameters *
 ***************************/

/*
 * Output at the terminal is is x_chars wide by y_chars long
*/
int y_chars = 50;
int x_chars = 90;

/*
 * The part of the complex plane to be drawn:
 * upper left corner is (xmin, ymax), lower right corner is (xmax, ymin)
*/
double xmin = -1.8, xmax = 1.0;
double ymin = -1.0, ymax = 1.0;

/*
 * Every character in the final output is
 * xstep x ystep units wide on the complex plane.
 */
double xstep;
double ystep;

struct Proc_Card {
	int turn,NPROCS;
	pid_t p;
	sem_t * arr;
};

typedef struct Proc_Card * proc_ptr;
sem_t *sema;

int safe_atoi(char *s, int *val){
	long l;
	char *endp;

	l = strtol(s,&endp,10);
	if( s!=endp && *endp =='\0'){
		*val = l;
		return 0;
	}
	else return -1;
}


void * safe_malloc(size_t size){
	void *p;

	if((p = malloc(size))== NULL){
		fprintf(stderr, "Out of memory, failed to allocate %zd bytes\n",
			size);
		exit(1);
	}
	return p;
}




void usage(char *argv0) {
	fprintf(stderr, "Usage: %s proc_count array_size\n\n"
		"Exactly one argument required:\n"
		" proc_count: The number of processes to create.\n",
		argv0);
	exit(1);
}



/*
 * This function computes a line of output
 * as an array of x_char color values.
 */
void compute_mandel_line(int line, int color_val[])
{
	/*
	 * x and y traverse the complex plane.
	 */
	double x, y;

	int n;
	int val;

	/* Find out the y value corresponding to this line */
	y = ymax - ystep * line;

	/* and iterate for all points on this line */
	for (x = xmin, n = 0; n < x_chars; x+= xstep, n++) {

		/* Compute the point's color value */
		val = mandel_iterations_at_point(x, y, MANDEL_MAX_ITERATION);
		if (val > 255)
			val = 255;

		/* And store it in the color_val[] array */
		val = xterm_color(val);
		color_val[n] = val;
	}
}

/*
 * This function outputs an array of x_char color values
 * to a 256-color xterm.
 */
void output_mandel_line(int fd, int color_val[])
{
	int i;

	char point ='@';
	char newline='\n';

	for (i = 0; i < x_chars; i++) {
		/* Set the current color, then output the point */
		set_xterm_color(fd, color_val[i]);
		if (write(fd, &point, 1) != 1) {
			perror("compute_and_output_mandel_line: write point");
			exit(1);
		}
	}

	/* Now that the line is done, output a newline character */
	if (write(fd, &newline, 1) != 1) {
		perror("compute_and_output_mandel_line: write newline");
		exit(1);
	}
}

void compute_and_output_mandel_line(void * p)
{
	/*
	 * A temporary array, used to hold color values for the line being drawn
	 */
	int color_val[x_chars];

	proc_ptr ptr=(proc_ptr) p;

	int turn = (ptr)->turn;
	int fd = 1;
	int line;
	int Num_Procs = (ptr)->NPROCS;
	sem_t * arr = (ptr)->arr;

	for(line = turn; line < y_chars; line+=Num_Procs){
		
	  compute_mandel_line(line , color_val);
	  sem_wait(&arr[(line)%Num_Procs]); //wait until one thread wakes you

	  //Start of Critical Seciton

	  output_mandel_line(fd , color_val);  //print the your line
	  //End of Critical Section
	  //printf("Process: %d,inside critical,after print\n",line);

	  sem_post(&arr[(line +1)% Num_Procs]);  //wake the thread for the next line
	  	  //printf("Process: %d,outside critical\n",line);

	}

	exit(1);
}

/*
 * Create a shared memory area, usable by all descendants of the calling
 * process.
 */
#define PROT (PROT_READ | PROT_WRITE)
#define FLAGS (MAP_SHARED | MAP_ANONYMOUS)

void *create_shared_memory_area(unsigned int numbytes)
{
	int pages;
	void *addr;

	if (numbytes == 0) {
		fprintf(stderr, "%s: internal error: called for numbytes == 0\n", __func__);
		exit(1);
	}

	/*
	 * Determine the number of pages needed, round up the requested number of
	 * pages
	 */
	pages = (numbytes - 1) / sysconf(_SC_PAGE_SIZE) + 1;


	if((addr = mmap(NULL,pages*sysconf(_SC_PAGE_SIZE),
			 PROT,FLAGS ,-1,0))==MAP_FAILED){
		perror("Problem with memory");
		exit(-1);
	}

	return addr;
}

void destroy_shared_memory_area(void *addr, unsigned int numbytes) {
	int pages;

	if (numbytes == 0) {
		fprintf(stderr, "%s: internal error: called for numbytes == 0\n", __func__);
		exit(1);
	}

	/*
	 * Determine the number of pages needed, round up the requested number of
	 * pages
	 */
	pages = (numbytes - 1) / sysconf(_SC_PAGE_SIZE) + 1;

	if (munmap(addr, pages * sysconf(_SC_PAGE_SIZE)) == -1) {
		perror("destroy_shared_memory_area: munmap failed");
		exit(1);
	}
}

int main(int argc,char *argv[])
{

	int line,NPROCS;
	proc_ptr ptr;
	pid_t p;
	if (argc != 2) usage(argv[0]);

	if (safe_atoi(argv[1], &NPROCS) < 0 || NPROCS <= 0) { //convert input string to integer by using "atoi"
		fprintf(stderr, "`%s' is not valid for `process_count'\n", argv[1]);
		exit(1);
	}


	xstep = (xmax - xmin) / x_chars;
	ystep = (ymax - ymin) / y_chars;

	if (NPROCS > y_chars ){//if the threads are more than the lines then compress them
	   	NPROCS = y_chars;
	}


	ptr = safe_malloc(NPROCS * sizeof(*ptr));


	sema = create_shared_memory_area(NPROCS * sizeof(*sema)); //semaphroes array
	
	unsigned i;

	for (i = 1; i < NPROCS; ++i){//for loop that initializes all apart from the first semaphore

		if((sem_init(&sema[i],1,0) == -1)){
			fprintf(stderr,"Error with semaphores initialization");

		}
	}

	sem_init(&sema[0],1,1);//semaphore of first thread is equal to one so it can be in critical section
	//prints(sema,NPROCS);


	//printf("Before fork\n");

	for (i = 0; i < NPROCS; ++i){

		if((p = fork())<0){
			perror("fork");
			exit(-1);
		}

		if(p > 0){
			continue;
		}

		if(p==0){
		//	printf("Child %d\n",i);
			ptr[i].NPROCS = NPROCS;

			ptr[i].turn = i;
			ptr[i].arr = sema;

			compute_and_output_mandel_line((void*) (ptr + i));
		}
	}

	//printf("father\n");

	/*
	 * draw the Mandelbrot Set, one line at a time.
	 * Output is sent to file descriptor '1', i.e., standard output.
	 */
	int status;

	for (i = 0; i < NPROCS; ++i){//wait to join
//		printf("repeat %d\n",i);
		wait(&status);
	}
	
	
	
	//TODO UNMAP



	//printf("end\n");
	for (i = 0; i < NPROCS; ++i){//destroy every semaphore
		sem_destroy(&sema[i]);
	}

	reset_xterm_color(1);

	return 0;
}
