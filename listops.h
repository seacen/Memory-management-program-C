/* =====================================================================
   Program written by Alistair Moffat, as an example for the book
   "Programming, Problem Solving, and Abstraction with C", Pearson
   Custom Books, Sydney, Australia, 2002; revised edition 2012,
   ISBN 9781486010974.

   See http://www.csse.unimelb.edu.au/~alistair/ppsaa/ for further
   information.

   Prepared December 2012 for the Revised Edition.
   ================================================================== */
/*
** Modified by Seacen April 2015
*/

/* polymorphic data structure */
typedef struct {
	int id;
	int size;
	int mem_loc;	/* where in memory is the unit located */
	int turn_num;	/* PROCESS ONLY in which turn which the process was loaded into memory */
	int swap_count;	/* PROCESS ONLY number of times the process has been swapped from memory */

} data_t;

#define LIST_PRINT_FORMAT "%d "

typedef struct node node_t;
/* one process node */
struct node {
	data_t data;
	node_t *next;

};

/* one waiting queue of processes */
typedef struct {
	node_t *head;
	node_t *foot;
	node_t *finger;
	node_t *curr;   /* for iterator */
	unsigned int size;
	node_t *prevHole;

} list_t;


list_t *make_empty_list(void);                    /* make an empty list */
int is_empty_list(list_t *list);                  /* return 1 is list is empty, 0 otherwise */
void free_list(list_t *list);                     /* free all memory in list */
void insert_at_head(list_t *list, data_t value);  /* insert value at head */
void insert_at_foot(list_t *list, data_t value);  /* insert value at foot */
data_t get_head(list_t *list);                    /* return data in head */
data_t get_foot(list_t *list);                    /* return data in foot */
void print_list(list_t *list);                    /* print the list */
data_t remove_foot(list_t *list);                 /* remove foot and return data in it */
data_t remove_head(list_t *list);                 /* remove head and return data in it */
void split_list(list_t *list, list_t **first, list_t **last);   /* split list on position of iterator */
data_t *begin_iterator(list_t *list);             /* set iterator to head and return data there */
data_t *step_iterator(list_t *list);              /* step iterator one node and return data there */
unsigned int list_size(list_t *list);             /* return list size */
list_t *join_lists(list_t *list1, list_t *list2); /* join list2 to end of list1 */
