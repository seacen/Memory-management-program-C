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
** Modified by Seacen Apirl 2015
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


/*
** Return a new list
*/
list_t
*make_empty_list(void) {
    list_t *list;
    list = (list_t*)malloc(sizeof(*list));
    assert(list!=NULL);
    list->head = list->foot = list->curr = NULL;
    list->finger = NULL;
    list->size = 0;
    return list;
}

/*
** Return 1 if list is empty, 0 otherwise
*/
int
is_empty_list(list_t *list) {
    assert(list!=NULL);
    return list->size == 0;
}

/*
** Take out the foot node of the list and return the data in it.
*/
data_t
remove_foot(list_t *list) {
    node_t *curr, *prev;
    assert(list!=NULL);
    curr = list->head;
    while (curr != list->foot) {
        prev = curr;
        curr = curr->next;
    }
    if (curr == list->head) /* only one item in the list */
        list->head = list->foot = NULL;
    else {
        prev->next = NULL;
        list->foot = prev;
    }
    
    data_t d = curr->data;

    list->size--;
    free(curr);

    return d;
}

/*
** free all memory used by list, incuding itself
*/
void
free_list(list_t *list) {
    node_t *curr, *prev;
    assert(list!=NULL);
    curr = list->head;
    while (curr) {
        prev = curr;
        curr = curr->next;
        free(prev);
    }
    free(list);
}

/*
** print the size of the list at the start, then
** print the data elements in each node (using LIST_PRINT_FORMAT)
** and ->NULL at the end of the list.
*/
void
print_list(list_t *list) {
    node_t *curr;
    assert(list!=NULL);
    printf("(size= %d)\n", list->size);
    curr = list->head;
    while (curr) {
        char extra[3] = "  ";
        if (curr == list->foot)
            extra[0] = 'f';
        if (curr == list->curr)
            extra[1] = 'c';
        printf("%s"LIST_PRINT_FORMAT", ", extra,curr->data.id);
		printf("%d, %d, %d, %d\n", curr->data.size, curr->data.mem_loc, curr->data.turn_num, curr->data.swap_count);
        curr = curr->next;
    }
    printf("->NULL\n");
}

/*
** Insert value in a new node at the head of the list
*/
void
insert_at_head(list_t *list, data_t value) {
    node_t *newnode;
    newnode = (node_t*)malloc(sizeof(*newnode));
    assert(list!=NULL && newnode!=NULL);
    newnode->data = value;
    newnode->next = list->head;
    list->head = newnode;
    if (list->foot==NULL) {
        /* this is the first insertion into the list */
        list->foot = newnode;
    }
    list->size++;
}

/*
** Insert value in a new node at the foot of the list
*/
void
insert_at_foot(list_t *list, data_t value) {
    node_t *newnode;
    newnode = (node_t*)malloc(sizeof(*newnode));
    assert(list!=NULL && newnode!=NULL);
    newnode->data = value;
    newnode->next = NULL;
    if (list->foot==NULL) {
        /* this is the first insertion into the list */
        list->head = list->foot = newnode;
    } else {
        list->foot->next = newnode;
        list->foot = newnode;
    }
    list->size++;
}

/*
** Return head value 
** ASSUMES: non NULL list
*/
data_t
get_head(list_t *list) {
    assert(list!=NULL && list->head!=NULL);
    return list->head->data;
}

/*
** Return foot value 
** ASSUMES: non NULL list
*/
data_t
get_foot(list_t *list) {
    assert(list!=NULL && list->foot!=NULL);
    return list->foot->data;
}

/*
** Remove head from list and return value
*/
data_t
remove_head(list_t *list) {
    assert(list!=NULL && list->head!=NULL);
    node_t *oldhead = list->head;
    data_t data  = oldhead->data;

    list->head = oldhead->next;
    if (list->head == NULL)     /* removed the last item from list */
        list->foot = NULL;

    free(oldhead);
    list->size--;
    return data;
}

/*
** Split the list into two parts based on list->curr
**   - all of the nodes before (not including) list->curr (*first)
**   - all of the nodes after (not including) list->curr (*last)
** If list->curr == NULL, *first is the entire list and *last is empty list
** SIDE_EFFECT: destroys the original list structure
** ASSUMES: list is not NULL
*/
void
split_list(list_t *list, list_t **first, list_t **last) {
    node_t *curr, *prev;
    assert(list!=NULL);

        /* find prev to list->curr */
        /* and count number of items before curr */
    int count = 0;
    curr = list->head;
    while (curr && curr != list->curr) {
        prev = curr;
        curr = curr->next;
        count++;
    }
	*first = make_empty_list();
    if (curr != list->head) {
        (*first)->size = count;
		(*first)->head = list->head;
        (*first)->foot = prev;
        prev->next     = NULL;
    }
	*last = make_empty_list();
    if (curr != list->foot) {
        (*last)->head = curr->next;
		(*last)->foot = list->foot;
		(*last)->size = list->size - count - 1;
    }
}

/*
** Begin the iterator (setting list->curr to list->head)
** and return the data in list->head
** Return NULL if list is empty;
*/
data_t *
begin_iterator(list_t *list) {
    assert(list != NULL);
    if (list->head == NULL)
        return NULL;
    
    list->curr = list->head;

    return &(list->head->data);
}

/*
** return data in list->curr and move list->curr along one node
** return NULL if list->curr is NULL
*/
data_t *
step_iterator(list_t *list) {
    assert(list != NULL);

    if (list->curr == NULL)
        return NULL;

    if (list->curr->next == NULL)
        return NULL;

    list->curr = list->curr->next;

    return &(list->curr->data);
}

/* return size of list */
unsigned int 
list_size(list_t *list) { 
    assert(list != NULL); 
    return list->size;
}

/*
** Make a new list with list2 on the end of list1
** RETURNS: the new list
** ASSUMES: list1 and list2 are not NULL
** SIDE_EFFECT: Alters list1 and aliases list2.
*/
list_t *
join_lists(list_t *list1, list_t *list2) {
    assert(list1 != NULL);
    assert(list2 != NULL);

    list_t *newlist;

    if (is_empty_list(list1)) {
        newlist = list2;
    }
	else if (is_empty_list(list2)) {
		newlist = list1;
	}
	else {
        newlist = make_empty_list();
        newlist->head = list1->head;
        newlist->foot = list2->foot;
        list1->foot->next = list2->head;
        newlist->size = list_size(list1) + list_size(list2);
    }

    return newlist;
}
