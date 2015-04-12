/* Memory mangement program
*
* Written by Xichang Zhao , April 2015
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include <limits.h>
#include <math.h>
#include "listops.h"
#include "listops.c"


#define ALGO_LEN		5		/* max string size for algorithm name */
#define NUM_ARGUMENT	7		/* number of command line arguments 
															to execute the program */
#define NUM_OPTIONS		3		/* number of options have to be there in arguments */
#define LAST_OPTPOS		5		/* index position of the last option */
#define LINELEN			100		/* max line length for the input */

#define NONE			'\0'	/* initial empty char */
#define TRUE			1		/* boolean true */
#define FALSE			0		/* boolean false */
#define DISABLED		-2		/* variable assigning to this is disabled */
#define UNACTIVATED		-1		/* variable assigning to this is in its initial state */



/****************************************************************/

/* function prototypes */

char* getOpt(int argc,char **argv,char *fname,char *algo,int *mem_size);
void read_file(char *fname,list_t *queue);
int read_line(char *line, int maxlen, FILE *f);
void loadToData(char* line, data_t *data);
void printData(data_t data);
void managingMemory(list_t *queue, list_t *freelist, list_t *memlist,
						int(*findHole)(int, data_t*, list_t**), int *swaps);
void initHole(list_t *freelist, int mem_size);
int findHoleFirst(int size, data_t *hole, list_t **freelist);
int findHoleBest(int size, data_t *hole, list_t **freelist);
int findHoleWorst(int size, data_t *hole, list_t **freelist);
int findHoleNext(int size, data_t *hole, list_t **freelist);
void swapOut(list_t **memlist, list_t **freelist, list_t *queue);
void updateProcess(list_t *memlist, data_t *process, int turn, int mem_loc);
void insert_data(data_t data, list_t *list,
								int(*cmp)(data_t current,data_t insert));
int memCmp(data_t current, data_t insert);
int freeCmp(data_t current, data_t insert);
data_t getProcess(list_t *memlist, int *count);
void removeProcess(list_t **memlist, int id, int count);
void checkAndMerge(list_t **freelist);
void printOutput(int id, list_t *memlist, int numHoles, int memorySize);
void updateHole(list_t **freelist, data_t hole, int size);
void findExtreme(int size, data_t *hole, list_t **freelist,
							int *found, int initBound, int(*cmp)(int, int));
int cmpMin(int min, int curr);
int cmpMax(int max, int curr);

/****************************************************************/

/* main program controls all the action
*/
int
main(int argc, char **argv) {
	
	char *fname=NULL;
	char algo[ALGO_LEN + 1];
	int mem_size,swaps = 0;
	list_t *queue,*freelist,*memlist;
	FILE *f;
	char num[20];

	fname=getOpt(argc,argv,fname,algo,&mem_size);

	/* waiting queue of processes */
	queue = make_empty_list();
	/* list of free memory holes */
	freelist = make_empty_list();
	/* list of processes in memory */
	memlist = make_empty_list();

	read_file(fname, queue);
	initHole(freelist,mem_size);

	if (strcmp(algo, "first")==0) {
		managingMemory(queue, freelist, memlist,findHoleFirst,&swaps);
	}
	else if (strcmp(algo, "best") == 0) {
		managingMemory(queue, freelist, memlist, findHoleBest, &swaps);
	}
	else if (strcmp(algo, "worst") == 0) {
		managingMemory(queue, freelist, memlist, findHoleWorst, &swaps);
	}
	else if (strcmp(algo, "next") == 0) {
		managingMemory(queue, freelist, memlist, findHoleNext, &swaps);
	}
	else {
		printf("algorithm input is invalid");
		exit(0);
	}

	printf("%d", swaps);

	f = fopen("result.txt", "a");
	fprintf(f, algo);
	fputc(' ', f);
	fprintf(f, fname);
	fputc('\n', f);
	sprintf(num, "%d", swaps);
	fprintf(f, num);
	fputc('\n', f);
	fclose(f);

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
			printf("input line size is overlimit (%d).",LINELEN);
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
	char *size,id[strlen(line)];
	int sizeofsize,i=0,x=0;

	/* create id string */
	while (line[x] != ' ') {
		id[x] = line[x];
		x++;
	}
	id[x] = '\0';

	data->id = atoi(id);

	sizeofsize = strlen(line) - strlen(id)-1;
	size = (char*)malloc(sizeofsize + 1);

	x++;
	for (i = 0; i < sizeofsize; i++) {
		size[i] = line[x];
		x++;
	}
	size[i] = '\0';

	data->size = atoi(size);
	/* initial value, not in memory yet */
	data->mem_loc = UNACTIVATED;
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

	hole.id = DISABLED;
	hole.size = mem_size;
	hole.mem_loc = 0;
	/* below two never going to be used */
	hole.turn_num = DISABLED;
	hole.swap_count = DISABLED;

	insert_at_foot(freelist,hole);
}

/****************************************************************/

/*doing memory mangement process
*/
void managingMemory(list_t *queue, list_t *freelist, list_t *memlist,
							int (*findHole)(int,data_t*,list_t**),int *swaps) {
	data_t hole,process;
	int turn = 0;

	while (!is_empty_list(queue)) {

		/* get the top process in waiting queue */
		process = remove_head(queue);

		/* if cannot find a hole, do the swapping until can */
		while (!findHole(process.size, &hole, &freelist)) {
			swapOut(&memlist, &freelist, queue);
			(*swaps)++;
			assert(freelist->foot != NULL);
		}

		updateProcess(memlist,&process,turn,hole.mem_loc);

		/* number of times processes have been added */
		turn++;
	}
}

/****************************************************************/

/*find a hole using first fit algorithm
*/
int findHoleFirst(int size, data_t *hole, list_t **freelist) {
	int i,found=FALSE;

	for (i = 0; i < (*freelist)->size; i++) {
		if (i == 0) {
			*hole= *begin_iterator(*freelist);
		}
		else {
			*hole = *step_iterator(*freelist);
		}
		/* if hole is big enough, break at the first found */
		if (hole->size >= size) {
			found = TRUE;
			/* remove and add remaining hole */
			updateHole(freelist, *hole, size);
			break;
		}
	}

	return found;
}

/****************************************************************/

/*find a hole using best fit algorithm
*/
int findHoleBest(int size, data_t *hole, list_t **freelist) {
	int found=FALSE;

	findExtreme(size, hole, freelist, &found, INT_MAX, cmpMin);

	return found;
}

/****************************************************************/

/*find either the smallest or the biggest hole according to cmp function
*/
void findExtreme(int size, data_t *hole, list_t **freelist, 
							int *found, int initBound, int(*cmp)(int,int)) {
	int i, extSize = initBound;

	for (i = 0; i < (*freelist)->size; i++) {
		if (i == 0) {
			*hole = *begin_iterator(*freelist);
		}
		else {
			*hole = *step_iterator(*freelist);
		}
		if (hole->size >= size) {
			*found = TRUE;
			if (cmp(extSize,hole->size)<0) {
				extSize = hole->size;
			}
		}
	}

	/* iterate to see if there is multiple extreme size hole, get the 
														first one in list */
	for (i = 0; i < (*freelist)->size; i++) {
		if (i == 0) {
			*hole = *begin_iterator(*freelist);
		}
		else {
			*hole = *step_iterator(*freelist);
		}
		if (hole->size == extSize) {
			updateHole(freelist, *hole, size);
			break;
		}
	}
}

/****************************************************************/

/*return negative if current hole size is smaller than prev min
*/
int cmpMin(int min, int curr) {
	return (curr - min);
}

/****************************************************************/

/*return negative if current hole size is bigger than prev min
*/
int cmpMax(int max, int curr) {
	return (max - curr);
}

/****************************************************************/

/*remove found hole and add remaining back to freelist
*/
void updateHole(list_t **freelist, data_t hole, int size) {
	data_t left_hole;
	list_t **tmp_first, **tmp_last;

	tmp_first = (list_t**)malloc(sizeof(*tmp_first));
	tmp_last = (list_t**)malloc(sizeof(*tmp_last));

	/* remove the hole from list */
	split_list(*freelist, tmp_first, tmp_last);
	*freelist = join_lists(*tmp_first, *tmp_last);

	/* set list.prevHole for storing information for next-fit algorithm */
	(*freelist)->prevHole = (*tmp_first)->foot;

	/* if hole size bigger than process size, add back remaining */
	if (hole.size > size) {
		left_hole.id = hole.id;
		left_hole.size = (hole.size - size);
		left_hole.mem_loc = hole.mem_loc + size;
		left_hole.turn_num = left_hole.swap_count = DISABLED;

		/* insert hole in ascending order of mem_loc */
		insert_data(left_hole, *freelist, freeCmp);
	}
}

/****************************************************************/

/*find a hole using worst fit algorithm
*/
int findHoleWorst(int size, data_t *hole, list_t **freelist) {
	int found = FALSE;

	findExtreme(size, hole, freelist, &found, 0, cmpMax);

	return found;
}

/****************************************************************/

/*find a hole using next fit algorithm
*/
int findHoleNext(int size, data_t *hole, list_t **freelist) {
	int found = FALSE,i;
	data_t *tmp;

	/* set the curr value for iteration start point */
	(*freelist)->curr = (*freelist)->prevHole;

	for (i = 0; i < (*freelist)->size; i++) {

		/* loop from the previous found hole pos, if NULL, loop from start */
		if ((tmp = step_iterator(*freelist)) == NULL) {
			*hole = *begin_iterator(*freelist);
		}
		else {
			*hole = *tmp;
		}
		if (hole->size >= size) {
			found = TRUE;
			updateHole(freelist, *hole, size);
			break;
		}
	}

	return found;
}

/****************************************************************/

/*swap out a process from memory
*/
void swapOut(list_t **memlist, list_t **freelist, list_t *queue) {
	data_t process,hole;
	int homoCount;		/* count how many equal max size process */

	process=getProcess(*memlist,&homoCount);

	removeProcess(memlist, process.id,homoCount);

	/* insert vacant hole to freelist */
	hole = process;
	hole.id = DISABLED;
	hole.turn_num = DISABLED;
	hole.swap_count = DISABLED;

	insert_data(hole,*freelist,freeCmp);

	/* check to see if there is adjacent holes to merge */
	checkAndMerge(freelist);

	process.mem_loc = UNACTIVATED;
	process.swap_count++;

	if (process.swap_count != 3) {
		insert_at_foot(queue, process);
	}
}

/****************************************************************/

/*check adjacent holes see if they can be merged
*/
void checkAndMerge(list_t **freelist) {
	int i,init_size,prevMerged=FALSE;
	data_t prev, curr;
	node_t *nprev,*ncurr;
	list_t **tmp_first, **tmp_last;

	/* for spliting list */
	tmp_first = (list_t**)malloc(sizeof(*tmp_first));
	tmp_last = (list_t**)malloc(sizeof(*tmp_last));

	/* iterate initial freelist size times, since it might 
										got trancated along the way */
	init_size = (*freelist)->size;
	for (i = 0; i < init_size; i++) {
		if (i == 0) {
			prev = curr = *begin_iterator(*freelist);
			ncurr = (*freelist)->curr;
		}
		else {
			/* if the previous iteration merging did not happen */
			if (!prevMerged) {
				prev = curr;
				nprev = ncurr;
			}
			/* set iteration point to the correct pos */
			(*freelist)->curr = nprev;
			curr = *step_iterator(*freelist);
			ncurr = (*freelist)->curr;
		}
		/* if adajcent holes found */
		if ((prev.mem_loc + prev.size) == curr.mem_loc) {
			prevMerged = TRUE;
			nprev->data.size += curr.size;
			prev.size += curr.size;

			/* remove the empty merged hole */
			split_list(*freelist, tmp_first, tmp_last);
			*freelist = join_lists(*tmp_first, *tmp_last);
		}
		else {
			prevMerged = FALSE;
		}
	}
}

/****************************************************************/

/*remove the process from memory
*/
void removeProcess(list_t **memlist, int id, int count) {
	int i;
	data_t curr;
	list_t **tmp_first, **tmp_last;

	tmp_first = (list_t**)malloc(sizeof(*tmp_first));
	tmp_last = (list_t**)malloc(sizeof(*tmp_last));

	/* find process by id and remove */
	for (i = 0; i < count; i++) {
		if (i == 0) {
			curr = *begin_iterator(*memlist);
		}
		else {
			curr = *step_iterator(*memlist);
		}
		if (curr.id == id) {
			split_list(*memlist, tmp_first, tmp_last);
			*memlist = join_lists(*tmp_first,*tmp_last);
			break;
		}
	}
}

/****************************************************************/

/*get the right process to swap out, return the process
*/
data_t getProcess(list_t *memlist, int *count) {
	data_t largest, curr, larges[memlist->size];
	int i, minTurn = INT_MAX, maxI;
	
	/* since memlist is a ordered list by size, head is the max size one */
	*count = 0;

	/* check if there is multiple max size process */
	for (i = 0; i < memlist->size; i++) {
		if (i == 0) {
			curr = largest = *begin_iterator(memlist);
		}
		else {
			curr = *step_iterator(memlist);
		}
		if (curr.size != largest.size) {
			break;
		}
		larges[i] = curr;
	}
	*count = i;

	/* find the one with lowest turn (oldest existence in memory) */
	for (i = 0; i < *count; i++) {
		if (larges[i].turn_num < minTurn) {
			minTurn = larges[i].turn_num;
			maxI = i;
		}
	}

	return larges[maxI];

}

/****************************************************************/

/*update process state
*/
void updateProcess(list_t *memlist, data_t *process, int turn, int mem_loc) {
	process->turn_num = turn;
	process->mem_loc = mem_loc;
	insert_data(*process,memlist,memCmp);
}

/****************************************************************/

/*print output to screen
*/
void printOutput(int id, list_t *memlist, int numHoles, int memorySize) {
	int memUsage,i;
	double memSum=0;
	data_t process;

	/* add up processes' size in memory */
	for (i = 0; i < memlist->size; i++) {
		if (i == 0) {
			process = *begin_iterator(memlist);
		}
		else {
			process = *step_iterator(memlist);
		}
		memSum += process.size;
	}
	memUsage = (int)ceil(100 * (memSum / ((double)memorySize)));

	printf("%d loaded, numprocesses=%d, ",id,memlist->size);
	printf("numholes=%d, memusage=%d%%\n",numHoles,memUsage);
}

/****************************************************************/

/*comparison function for descending ordering 
							list of processes in memory by size
*/
int memCmp(data_t current, data_t insert) {
	
	return (insert.size - current.size);
}

/****************************************************************/

/*comparison function for ascending ordering list of free holes by mem_loc
*/
int freeCmp(data_t current, data_t insert) {

	return (current.mem_loc + current.size - insert.mem_loc);
}

/****************************************************************/

/*insert data in its right position in list
*/
void insert_data(data_t data, list_t *list,int (*cmp)(data_t,data_t)) {
	int i;
	data_t tmp_data;
	node_t *prev=NULL,*newnode;

	for (i = 0; i < list->size; i++) {
		if (i == 0) {
			tmp_data = *begin_iterator(list);
		}
		else {
			tmp_data = *step_iterator(list);
		}

		if (cmp(tmp_data,data) <= 0) {
			prev = list->curr;
		}
		else {
			break;
		}
	}

	/* no hole in list before the inserted hole */
	if (prev == NULL) {
		insert_at_head(list, data);
	}
	/* no hole in list after the inserted hole*/
	else if (list->curr == prev) {
		insert_at_foot(list, data);
	}
	/* in between */
	else {
		newnode = (node_t*)malloc(sizeof(*newnode));
		newnode->data = data;
		prev->next = newnode;
		newnode->next = list->curr;
		(list->size)+=1;
	}
}
