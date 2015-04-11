/* Memory mangement program
*
* Written by Xichang Zhao , April 2015
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include "listops.h"
#include "listops.c"


#define ALGO_LEN		5		/* max string size for algorithm name */
#define NONE			'\0'	/* initial empty char */
#define NUM_ARGUMENT	7		/* number of command line arguments 
															to execute the program */
#define NUM_OPTIONS		3		/* number of options have to be there in arguments */
#define LAST_OPTPOS		5		/* index position of the last option */
#define LINELEN			40		/* max line length for the input */
#define PRE_INPUT		2		/* number of input characters before the size integer */



/****************************************************************/

/* function prototypes */

char* getOpt(int argc,char **argv,char *fname,char *algo,int *mem_size);
void read_file(char *fname,list_t *queue);
int read_line(char *line, int maxlen, FILE *f);
void loadToData(char* line, data_t *data);
void printData(data_t data);
void managingMemory(list_t *queue, list_t *freelist, list_t *memlist, 
						int (*findHole)(data_t, data_t*, list_t*, list_t*));
void initHole(list_t *freelist, int mem_size);
int findHoleFirst(data_t process, data_t *hole,list_t *freelist,list_t *memlist);
int findHoleBest(data_t process, data_t *hole,list_t *freelist,list_t *memlist);
int findHoleWorst(data_t process, data_t *hole,list_t *freelist,list_t *memlist);
int findHoleNext(data_t process, data_t *hole,list_t *freelist,list_t *memlist);
void swapProcess(list_t *memlist, list_t *freelist, list_t *queue);
void updateProcess(list_t *memlist, data_t *process, int turn, int mem_loc);

/****************************************************************/

/* main program controls all the action
*/
int
main(int argc, char **argv) {
	
	char *fname=NULL;
	char algo[ALGO_LEN + 1];
	int mem_size;
	list_t *queue,*freelist,*memlist;

	fname=getOpt(argc,argv,fname,algo,&mem_size);
	printf("%s\n%s\n%d\n", fname, algo, mem_size);

	/* waiting queue of processes */
	queue = make_empty_list();
	/* list of free memory holes */
	freelist = make_empty_list();
	/* list of processes in memory */
	memlist = make_empty_list();

	read_file(fname, queue);
	/*print_list(queue);*/
	initHole(freelist,mem_size);
	/*print_list(freelist);*/


	if (strcmp(algo, "first")==0) {
		managingMemory(queue, freelist, memlist,findHoleFirst);
	}
	else if (strcmp(algo, "best") == 0) {
		managingMemory(queue, freelist, memlist, findHoleBest);
	}
	else if (strcmp(algo, "worst") == 0) {
		managingMemory(queue, freelist, memlist, findHoleWorst);
	}
	else if (strcmp(algo, "next") == 0) {
		managingMemory(queue, freelist, memlist, findHoleNext);
	}
	else {
		printf("algorithm input is invalid");
		exit(0);
	}
	return 0;
}

/****************************************************************/

/*get the options, return file name
*/
char* getOpt(int argc, char **argv, char *fname, char *algo, int *mem_size) {

	char *options[] = { "-f", "-a", "-m" };
	int found = 0,i,x;

	if (argc != NUM_ARGUMENT) {
		printf("command line arguments are invalid.");
		exit(0);
	}
	/* for each available option */
	for (x = 0; x < NUM_OPTIONS; x++) {
		/* for each option in command arguments */
		for (i = 1; i < LAST_OPTPOS+1; i += 2) {
			/* if find match */
			if (strcmp(options[x],argv[i] )==0) {
				found++;
				/* if match is -f */
				if (strcmp(options[x], "-f") == 0) {
					fname = (char*)malloc(strlen(argv[i+1])+1);
					strcpy(fname, argv[i + 1]);

				}
				/* -a */
				else if (strcmp(options[x], "-a") == 0) {
					assert(strlen(options[x]) <= ALGO_LEN);
					strcpy(algo, argv[i + 1]);
				}
				/* -m */
				else if (strcmp(options[x], "-m") == 0) {
					/* if memory size in input is 0 or not an int */
					if (!(*mem_size = atoi(argv[i+1]))) {
						printf("memory size is invalid.");
						exit(0);
					}
				}
			}
		}
	}
	if (found != 3) {
		printf("command line arguments are invalid.");
		exit(0);
	}
	return fname;
}

/****************************************************************/

/*read from the file, and load processes into the queue
*/
void read_file(char *fname, list_t *queue) {
	FILE *f;
	char line[LINELEN + 1];
	data_t process;

	f = fopen(fname, "r");
	if (f == NULL) {
		printf("fail to open the file");
		exit(0);
	}

	while (read_line(line, LINELEN,f)) {
		loadToData(line, &process);
		insert_at_foot(queue, process);
	}
}

/****************************************************************/

/*read a line from the file
*/
int
read_line(char *line, int maxlen, FILE *f) {
	int i = 0, c;
	while (((c = getc(f)) != EOF) && (c != '\n')) {
		if (i<maxlen) {
			line[i] = c;
			i++;
		}
		else {
			printf("input line size is overlimit (40).");
			exit(0);
		}
	}
	line[i] = '\0';
	return ((i>0) || (c != EOF));
}

/****************************************************************/

/*put line information to the data structure
*/
void loadToData(char* line, data_t *data) {
	char *size;
	int sizeofsize,i;

	sizeofsize = strlen(line) - PRE_INPUT;
	size = (char*)malloc(sizeofsize + 1);

	/* covnert char to int */
	data->id = line[0]-'0';

	for (i = 0; i < sizeofsize; i++) {
		size[i] = line[i + 2];
	}
	size[i] = '\0';

	data->size = atoi(size);
	/* initial value, not in memory yet */
	data->mem_loc = -1;
	data->turn_num = 0;
	data->swap_count = 0;
}

/****************************************************************/

/*print a data unit
*/
void printData(data_t data) {
	printf("id: %d, size: %d, mem_loc: %d, ",data.id, data.size, data.mem_loc);
	printf("turn_num: %d, swap_count: %d\n",data.turn_num,data.swap_count);
}

/****************************************************************/

/*initialise memory in freelist
*/
void initHole(list_t *freelist,int mem_size) {
	data_t hole;

	hole.id = 1;
	hole.size = mem_size;
	hole.mem_loc = 0;
	/* below two never going to be used */
	hole.turn_num = -2;
	hole.swap_count = -2;

	insert_at_foot(freelist,hole);
}

/****************************************************************/

/*doing memory mangement process
*/
void managingMemory(list_t *queue, list_t *freelist, list_t *memlist,
							int (*findHole)(data_t,data_t*,list_t*,list_t*)) {
	data_t hole,process;
	int turn = 0;
	while (!is_empty_list(queue)) {
		process = remove_head(queue);
		while (!findHole(process, &hole, freelist, memlist)) {
			swapProcess(memlist, freelist, queue);
		}
		updateProcess(memlist,&process,turn,hole.mem_loc);
		turn++;
	}

}

/****************************************************************/

/*find a hole using first fit algorithm
*/
int findHoleFirst(data_t process, data_t *hole, list_t *freelist, list_t *memlist) {
	return 1;
}

/****************************************************************/

/*find a hole using best fit algorithm
*/
int findHoleBest(data_t process, data_t *hole, list_t *freelist, list_t *memlist) {
	return 1;
}

/****************************************************************/

/*find a hole using worst fit algorithm
*/
int findHoleWorst(data_t process, data_t *hole, list_t *freelist, list_t *memlist) {
	return 1;
}

/****************************************************************/

/*find a hole using next fit algorithm
*/
int findHoleNext(data_t process, data_t *hole, list_t *freelist, list_t *memlist) {
	return 1;
}

/****************************************************************/

/*find a hole using next fit algorithm
*/
void swapProcess(list_t *memlist, list_t *freelist, list_t *queue) {

}

/****************************************************************/

/*update process state
*/
void updateProcess(list_t *memlist, data_t *process, int turn, int mem_loc) {
	process->turn_num = turn;
	process->mem_loc = mem_loc;
	insert_at_head(memlist, *process);
}