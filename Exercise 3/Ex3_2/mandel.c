
/*
 * mandel.c
 *
 * A program to draw the Mandelbrot Set on a 256-color xterm.
 *
 */
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <semaphore.h>
#include "mandel-lib.h"
#include <pthread.h>
#define MANDEL_MAX_ITERATION 100000

/***************************
 * Compile-time parameters *
 ***************************/

/*
 * Output at the terminal is is x_chars wide by y_chars long
*/

#define perror_pthread(ret,msg) \
	do { errno = ret; perror(msg); } while(0)


int y_chars = 50;
int x_chars = 90;

//pointer to the array of semaphores
sem_t *sema;

//struct for its thread
struct pthread_card{
        pthread_t  tid;
        int thrcnt;
        int fd;
        int turn;
};

typedef struct pthread_card * thr_ptr;

//Signal Handel for the signal SIGINT
void signal_handler(int sig ){
	reset_xterm_color(1); //reset the color
	exit(1);//terminate the program
}


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

void * compute_and_output_mandel_line(void * p)
{
	/*
	 * A temporary array, used to hold color values for the line being drawn
	 */
	int color_val[x_chars];

	thr_ptr ptr = (thr_ptr) p;   //type casting

	//take some informations from the thread struct
	int fd = ptr->fd;
	int step = ptr->thrcnt;
	int turn = ptr->turn;
	int line;

	//Let's print the image !!!
	for (line = turn; line < y_chars; line += step){
	  compute_mandel_line(line , color_val);

	  sem_wait(&sema[(line)%step]); //wait until one thread wakes you

	  //Start of Critical Seciton
	  output_mandel_line(fd , color_val);  //print the your line
	  //End of Critical Section

	  sem_post(&sema[(line +1)% step]);  //wake the thread for the next line
        }

    return NULL; //if you have printed the image then every thread
                //return from the function back to the program
}


int safe_atoi(char *s, int *val)
{
	long l;
	char *endp;

	l = strtol(s, &endp, 10);
	if (s != endp && *endp == '\0') {
		*val = l;
		return 0;
	} else
		return -1;
}


void *safe_malloc(size_t size)
{
	void *p;

	if ((p = malloc(size)) == NULL) {
		fprintf(stderr, "Out of memory, failed to allocate %zd bytes\n",
			size);
		exit(1);
	}

	return p;
}

void usage(char *argv0)
{
	fprintf(stderr, "Usage: %s thread_count array_size\n\n"
		"Exactly one  argument required:\n"
		"    thread_count: The number of threads to create.\n",
		argv0);
	exit(1);
}

//main function
int main(int argc, char * argv[])
{
	thr_ptr thr;      //thr ---> [ struct pthread_card ]
	int thrcnt,ret,i;

	signal(SIGINT,signal_handler);

	if (argc != 2) {//check for wrong input
	  	usage(argv[0]);
	}


	if (safe_atoi(argv[1], &thrcnt) < 0 || thrcnt <= 0) { //convert input string to integer by using "atoi"
		fprintf(stderr, "`%s' is not valid for `thread_count'\n", argv[1]);
		exit(1);
	}

	if (thrcnt > y_chars ){//if the threads are more than the lines then compress them
	   	thrcnt = y_chars;
	}

        xstep = (xmax - xmin) / x_chars;
        ystep = (ymax - ymin) / y_chars;


//	printf("We are going to compute Mandelbrot using %d\n",thrcnt);

	thr = safe_malloc(thrcnt * sizeof(*thr));//allocate space for the array of threads

//	printf("Memory Allocation for threads is OK ! \n");

	sema = safe_malloc(thrcnt * sizeof(*sema));//allocate space for the array of semaphores

//	printf("Memory Allocation for semaphores is OK!  \n");

	for (i = 1; i < thrcnt; ++i){//for loop that initializes all apart from the first semaphore

		if((sem_init(&sema[i],0,0) == -1)){
			fprintf(stderr,"Error with semaphores initialization");

		}
	}


	sem_init(&sema[0],0,1);//semaphore of first thread is equal to one so it can be in critical section

        for (i = 0; i < thrcnt; i++){//for-loop which create all the threads and fill their structures with informations

		thr[i].turn = i;
		thr[i].fd = 1;
		thr[i].thrcnt = thrcnt;

        	ret =  pthread_create(&thr[i].tid,NULL,compute_and_output_mandel_line,(void *) (thr+i));//create the thread and send it in a function (3 argument)

		if (ret<0) {
		perror_pthread(ret, "pthread_create");
		exit(1);

        	}
  	}


	for (i = 0; i < thrcnt; i++) {//loop that wait every thread to finish its work
		ret = pthread_join((thr+i)->tid, NULL);

		if (ret) {
			perror_pthread(ret, "pthread_join");
			exit(1);
		}
	}


	for (i = 0; i < thrcnt; ++i){//destroy every semaphore
		sem_destroy(&sema[i]);
	}

	reset_xterm_color(1);//reset the colors back

	return 0;//leave
}
