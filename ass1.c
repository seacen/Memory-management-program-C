/* Memory mangement program
*
* Written by Xichang Zhao , April 2015
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <assert.h>

#define ALGO_LEN		5		/* max string size for algorithm name */
#define NONE			'\0'	/* initial empty char */
#define NUM_ARGUMENT	7		/* number of command line arguments to execute the program */
#define NUM_OPTIONS		3		/* number of options have to be there in arguments */
#define LAST_OPTPOS		5		/* index position of the last option */


typedef struct {
	int id;
	int size;
	int mem_loc;	/* where in memory is the process located */
	int turn_num;	/* in which turn which the process was loaded in memory */

} process_t;

/* one process */

typedef struct hole hole_t;

struct hole {
	int mem_loc;
	int size;
	hole_t *next;

};

typedef struct {
	hole_t *head;
	hole_t *foot;

} list_t;


/****************************************************************/

/* function prototypes */

char* getOpt(int argc,char **argv,char *fName,char *algo,int *mem_size);

/****************************************************************/

/* main program controls all the action
*/
int
main(int argc, char **argv) {
	
	char *fName=NULL;
	char algo[ALGO_LEN + 1];
	int mem_size;

	fName=getOpt(argc,argv,fName,algo,&mem_size);
	printf("%s\n%s\n%d\n", fName,algo,mem_size);

	return 0;
}

/****************************************************************/

/*get the options, return file name
*/
char* getOpt(int argc, char **argv, char *fName, char *algo, int *mem_size) {

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
			if (strcmp(options[x],argv[i] )==0) {
				found++;
				if (strcmp(options[x], "-f") == 0) {
					fName = (char*)malloc(strlen(argv[i+1])+1);
					strcpy(fName, argv[i + 1]);

				}
				else if (strcmp(options[x], "-a") == 0) {
					strcpy(algo, argv[i + 1]);
				}
				else if (strcmp(options[x], "-m") == 0) {
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
	return fName;
}