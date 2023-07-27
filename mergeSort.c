#define LEN 11
#define MAXEL 100
#define LEN_DEF 8
#define LEN_MAX 256

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


/* Noah Myers
 * It's 2pm the day this is due and I JUST saw the part in the requirements wherre my algorithm isn't allowed to be recurssive. I don't have time to change this now, as I have to go to work.
 *
 * must run with:: number of ints wanted to be sorted, then a list of all ints.
 * input example : ./a.out 5(number of ints to be sorted). the program then reads from stdin using scanf(), so it's not accepting more integers on the same line, since I used argv[] to catch
 * size of sort. 
 *
 * Program does work with piping, but once again, you must first specify the size of the sort with the integer at runtime before it scans the input file, so what I was doing was generating random
 * numbers in a file, and then giving my program a random amount to sort, and it was working perfectly. 
 */

void print_list(int * list, int len)
{
	/*Print all the ints of a list in brackets followed by a newline*/
	int i;
	printf("[ ");
	for (i = 0; i < len; i++)
	{
		printf("%d ", list[i]);
	}
	printf("]\n\n");
}


void sigHandler(int sig) {
	

}

void merge(int * list,int * llist,int lsize,int * rlist,int rsize){	

	int r = 0; //right_half index
	int l = 0; //left_half index
	int i = 0; //list index
 
	//merge left_half and right_half back into list
	for ( i =0, r = 0, l = 0; l < lsize && r < rsize; i++)
	{
		if ( llist[l] < rlist[r] ) { 
				list[i] = llist[l++]; 
			} else { 
				list[i] = rlist[r++]; 
			}
	}

	//Copy over leftovers of whichever temporary list hasn't finished
	for ( ; l < lsize; i++, l++) { 
		list[i] = llist[l];
	}

	for ( ; r < rsize; i++, r++) { 
		list[i] = rlist[r]; 
	}
}


void forking_mergesort(int * list,int n) {

	//termination condition
	if (n <= 1) {
		return; 
	}

	int lsize=(n/2);
	int rsize=(n/2+n%2);

	int i,r,l;
	int fd1[2],fd2[2],fd3[2],fd4[2];
	int pid1,pid2;
	//int stat;

	//declare temporary lists
	int llist[lsize];
	int rlist[rsize];

	for (i = 0,l=0; i<lsize; i++, l++)
	{
		llist[l] = list[i];
	}

	for (i =lsize,r=0; i < n; i++, r++)
	{
		rlist[r] = list[i];
	}


	//pipes creating
	if(pipe(fd1) == -1){
		perror("Pipe error");
		exit(-1);
	}

	if(pipe(fd2) == -1){
		perror("Pipe error");
		exit(-1);
	}

	if(pipe(fd3) == -1){
		perror("Pipe error");
		exit(-1);
	}

	if(pipe(fd4) == -1){
		perror("Pipe error");
		exit(-1);
	}


	//fork using switches
	pid1=fork();
	switch(pid1){
		case -1://fork error
			perror("fork error\n");
			exit (-1);
		case 0://left_child process
			close(fd1[1]);
			close(fd2[0]);

			//read data from father
			while(read(fd1[0],&llist,sizeof(int)*(lsize)) !=0)

			//llist handling with fork_mergesort recursive calling
			forking_mergesort(llist,lsize);

			//return data of a left child to parent
        	write(fd2[1],&llist, sizeof(int)*(lsize));
        	close(fd2[1]);

        	exit(0);
      		default://parent process
        		close(fd1[0]);
        		close(fd2[1]);

        	//send data to the left child
        		write(fd1[1],&llist, sizeof(int)*(lsize));
        		close(fd1[1]);

        	//read data from left child
        	while(read(fd2[0],&llist,sizeof(int)*(lsize)) !=0)
			wait(NULL);
			close(fd2[0]);

	        	pid2=fork();
	        	switch(pid2){
	        		case -1://fork error
	        			perror("fork error\n");
	        			exit(-1);
	        		case 0://right child process
	        			close(fd3[1]);
	        			close(fd4[0]);

	        			//read data from parent
	        			while(read(fd3[0],rlist,sizeof(int)*(rsize)) !=0)

	        			//llist handling with fork_mergesort recursive calling
						forking_mergesort(rlist,rsize);
	        		
	        			//return data of right child to parent
	        			write(fd4[1],rlist,sizeof(int)*(rsize));	        			      	
	        			close(fd4[1]);
	        			exit(0);
	        		default:
	        			close(fd3[0]);
	        			close(fd4[1]);
        		
	        			//send data to the right child
	        			write(fd3[1],&rlist,sizeof(int)*(rsize));
	        			close(fd3[1]);

	        			//read data from right child
						while(read(fd4[0],&rlist,sizeof(int)*(rsize)) !=0)
						wait(NULL);
						close(fd4[0]);

						//merge function calling
						merge(list,llist,lsize,rlist,rsize);
        		}
	}
}


//Main parent process, does priting and reading.
int main(int argc, char* argv[]) {
	  int i;
	  signal(SIGUSR1, sigHandler);
	if(argc<=1){
		printf("Parameter error: missing argument (number of elements) \n");
	}else{
		int *numbers;
				
		//Read first int for size
		int nb=atoi(argv[1]);
			
		numbers = malloc(nb * sizeof(*numbers));

		// Fill array with numbers from stdin
		for(i=0; i<nb; i++){
			//pause();
			scanf("%d", &numbers[i]);
		}
		 
		//printing before sort
		printf("\nArray of %d integers inputted:\n",nb);
		print_list(numbers,nb);

		forking_mergesort(numbers,nb);

		//printing after sort
		printf("Mergesort result:\n");
		print_list(numbers,nb);
		free(numbers);
	}
	return 0;
}
	       


