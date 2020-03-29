#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>


// Node type definition
typedef struct node{
    void* data;
    struct node* next;  // link to next
    struct node* prev;  // link to prev
    short isFree;
    int count;
} Node;

// LIST type definition
typedef struct {
    int count;      // Current number of elements
    Node* curr;     // Current element (head)
    Node* first;    // First element on the list
    Node* last;     // Last elemenet on the list
    short isFree;
} LIST;


/*
 * Make a new empty list and return its reference as LIST pointer
 * Return NULL if failed
 */
LIST* ListCreate(void);


/*
 * return the number of items on the list
 */
int ListCount(LIST* list);


/*
 * returns a pointer to the first element on the list
 * make the first element as the current element
 */
void *ListFirst(LIST* list);


/*
 * returns pointer to the last item
 * makes the last element as the current one
 */
void *ListLast(LIST* list);


/*
 * next item becomes the current item
 * return the current pointer
 * return NULL if next item beyond the list
 */
void *ListNext(LIST* list);


/*
 * previous item becomes the current item
 * return the current pointer
 * return NULL if prev item before the start of list
 */
void *ListPrev(LIST* list);


/*
 * returns the pointer to the current item in the list
 */
void *ListCurr(LIST* list);


/*
 * adds new item directly after the current item
 * new item becomes the current item
 * if current pointer is before the start, new item added to start
 * if current pointer after the end, new item added to the end
 * returns 0 for success. -1 for failure
 */
int ListAdd(LIST* list,void* item);


/*
* adds new item directly before the current item
* new item becomes the current item
* if current pointer is before the start, new item added to start
* if current pointer after the end, new item added to the end
* returns 0 for success. -1 for failure
*/
int ListInsert(LIST* list,void* item);


/*
 * adds new item to end of the list
 * new item becomes the curr item
 * return 0 for success, -1 for failure
 */
int ListAppend(LIST* list,void* item);


/*
 * adds new item before the start of the list
 * new item becomes the curr item
 * return 0 for success, -1 for failure
 */
int ListPrepend(LIST* list,void* item);


/*
 * returns the curr item and take it out of the list
 * next item becomes the curr item
 */
void *ListRemove(LIST* list);


/*
 * adds list 2 to end of list 1
 * list 2 no longer exists
 */
void ListConcat(LIST* list1,LIST* list2);


/*
 * delete list
 * itemFree is a pointer to a routine that frees
    an item.
 * invoked as (*itemFree)(itemToBeFreed);
 */
void ListFree(LIST* list, void (*itemFree)(Node*));


/*
 * return last item and take it out of the list
 * the new last item becomes the current item
 */
void *ListTrim(LIST* list);


/*
 * searches list starting at the current item until the end is reached
    or match is found.
 * Match is determined by the comparator parameter. This parameter
    is a pointer to a routine that takes as its first argument an
    item pointer, and as its second argument comparisonArg.
 * Comparator returns 0 if the item and comparisonArg don't match, 1 if they do.
 * Current pointer is left at the matched item and the pointer to that
    item is returned.
 * If no match, current pointer is left beyond end of list
    and a NULL pointer is returned.

 */
void *ListSearch(LIST* list, int (*comparator)(void*, void*), void* comparisonArg);

//------------------------------------------------------------------------------------

#define MAX_HEADS 10
#define MAX_NODES 200

LIST heads[MAX_HEADS];  // LIST static array
Node nodes[MAX_NODES];  // Node static array

/*
 * Helper function to print
 */
int printList(LIST *list) {
    if(list->isFree == 1) {
        printf("Free head\n\n");
        return 0;
    }
    if(list==NULL)
        return -1;

    Node* curr = list->first;
    if(curr == NULL) {
        printf("Empty List\n\n");
        return 0;
    }

    while(curr!=NULL) {

        printf("%d -> ", curr);
        curr = curr->next;
    }
    printf("~~NULL\n");

    curr = list->first;

    while(curr!=NULL) {

        printf("%d -> ", *((int *)(curr->data)));

        curr = curr->next;
    }
    printf("~~NULL\n\n");
    return 0;
}

/*
 *Helper functin to find empty spaces on nodes array
 *Finds empty nodes in O(n) time
 */
int findEmptyIndex(Node *collection) {
    if(nodes->count == MAX_NODES)
        return -1;
    for(int i=0; i<MAX_NODES; i++)
    {
        if(collection[i].isFree == 1)
        {
            return i;
        }
    }
    return -1;
}

/*
 Function to initialize all the heads and nodes
 */
void init(void) {

    for(int i=0; i<MAX_HEADS; i++) {
        heads[i].count=0;
        heads[i].curr = NULL;
        heads[i].first = NULL;
        heads[i].last = NULL;
        heads[i].isFree = 1;
    }

    for(int i=0; i<MAX_NODES; i++) {
        nodes[i].count = 0;
        nodes[i].data = 0;
        nodes[i].isFree = 1;
        nodes[i].next = NULL;
        nodes[i].prev = NULL;
    }

    return;
}

/*
 * Make a new, empty list and return its reference as LIST pointer
 * Return NULL if failed
 */
LIST* ListCreate(void) {
    LIST* newList = NULL; // initialization

    for(int i=0; i<MAX_HEADS; i++) {
        if(heads[i].isFree == 1) {
            newList = &heads[i];
            heads[i].isFree = 0;    // no more free
            return newList;         // return the new list if empty space allocated to it
        }
    }

    return NULL; // returns NULL if no available space
}


/*
 * return the number of items on the list
 */
int ListCount(LIST* list) {
    if(list->isFree==1)
        return 0;
    return list->count;
}


/*
 * returns a pointer to the first element on the list
 * make the first element as the current element
 */
void *ListFirst(LIST* list) {
    if(list==NULL || list->isFree==1) {
        return NULL;
    }
    return list->first;
}


/*
 * returns pointer to the last item
 * makes the last element as the current one
 */
void *ListLast(LIST* list) {
    if(list==NULL || list->isFree==1) {
        return NULL;
    }
    return list->last;
}


/*
 * next item becomes the current item
 * return the current pointer
 * return NULL if next item beyond the list
 */
void *ListNext(LIST* list) {
    if(list==NULL || list->isFree==1) {
        return NULL;
    }

    if(list->curr->next != NULL) {
        list->curr = list->curr->next;
        return list->curr;
    }

    return NULL;
}


/*
 * previous item becomes the current item
 * return the current pointer
 * return NULL if prev item before the start of list
 */
void *ListPrev(LIST* list) {
    if(list==NULL || list->isFree==1) {
        return NULL;
    }

    if(list->curr->prev != NULL) {
        list->curr = list->curr->prev;
        return list->curr;
    }

    return NULL;
}


/*
 * returns the pointer to the current item in the list
 */
void *ListCurr(LIST* list) {
    if(list==NULL || list->isFree==1) {
        return NULL;
    }

    if(list == NULL)
        return NULL;

    return list->curr;
}

/*
 * adds new item to end of the list
 * new item becomes the curr item
 * return 0 for success, -1 for failure
 */
int ListAppend(LIST* list,void* item) {
    if(item == NULL || list == NULL || list->isFree==1)
        return -1;

    int index = nodes->count;
    list->curr = list->last;
    if(nodes[index].isFree == 0)
        index = findEmptyIndex(nodes);

    if(nodes[index].isFree == 1) {
        nodes[index].data = item;
        nodes[index].isFree = 0;
        nodes[index].next = NULL;
        nodes[index].prev = list->curr;

        //empty list
        if(list->curr == NULL) {
            list->first = &nodes[index];
            list->last = list->first;
            list->first->prev = NULL;
        }

        //current element == last element
        else if(list->curr == list->last) {
            list->last = &nodes[index];
            list->curr->next = &nodes[index];
        }

        list->curr = &nodes[index];
        list->count++;
        nodes->count++;

        return 0;
    }
    return -1;
}


/*
 * adds new item before the start of the list
 * new item becomes the curr item
 * return 0 for success, -1 for failure
 */
int ListPrepend(LIST* list,void* item) {
    if(item == NULL || list == NULL || list->isFree==1)
        return -1;

    int index = nodes->count;
    list->curr = list->first;

    if(nodes[index].isFree == 0)
        index = findEmptyIndex(nodes);

    if(nodes[index].isFree == 1) {
        nodes[index].data = item;
        nodes[index].isFree = 0;
        nodes[index].next = list->curr;
        nodes[index].prev = NULL;

        if(list->curr == NULL) {
            //empty list
            list->first = &nodes[index];
            list->last = list->first;
        }
        else if(list->curr == list->first) {
            //current element == first element
            list->curr->prev = &nodes[index];
            list->first = &nodes[index];
        }

        list->curr = &nodes[index];
        list->count++;
        nodes->count++;

        return 0;
    }

    return -1;
}


/*
 * adds new item directly after the current item
 * new item becomes the current item
 * if current pointer is before the start, new item added to start
 * if current pointer after the end, new item added to the end
 * returns 0 for success. -1 for failure
 */
int ListAdd(LIST* list,void* item) {

    if(item == NULL || list == NULL || list->isFree==1)
        return -1;

    int index = nodes->count;
    if(nodes[index].isFree == 0)
        index = findEmptyIndex(nodes);

    if(nodes[index].isFree == 1) {
        // current element is last or list is empty
        if(list->curr == list->last || list->curr == NULL) {
            int result = ListAppend(list, item);
            return result;
        }
        else
        {
            nodes[index].data = item;
            nodes[index].isFree = 0;
            nodes[index].next = list->curr->next;
            nodes[index].prev = list->curr;
            list->curr->next = &nodes[index];
            list->curr = &nodes[index];
            (list->curr->next)->prev = &nodes[index];
            list->count++;
            nodes->count++;
            return 0;
        }
    }
    return -1;
}


/*
* adds new item directly before the current item
* new item becomes the current item
* if current pointer is before the start, new item added to start
* if current pointer after the end, new item added to the end
* returns 0 for success. -1 for failure
*/
int ListInsert(LIST* list,void* item) {
    if(item == NULL || list == NULL || list->isFree==1)
        return -1;

    int index = nodes->count;
    if(nodes[index].isFree == 0)
        index = findEmptyIndex(nodes);

    if(nodes[index].isFree == 1) {

        //empty list or current element == first element
        if(list->curr == NULL || list->curr == list->first) {
            return ListPrepend(list, item);
        }
        else {
            nodes[index].data = item;
            nodes[index].isFree = 0;
            nodes[index].next = list->curr;
            nodes[index].prev = list->curr->prev;
            (list->curr->prev)->next = &nodes[index];
            list->curr->prev = &nodes[index];
            list->curr = &nodes[index];
            list->count++;
            nodes->count++;
            return 0;
        }
    }

    return -1;
}

/*
 * returns the curr item and take it out of the list
 * next item becomes the curr item
 */
void *ListRemove(LIST* list) {
    if(list==NULL || list->isFree==1) {
        return NULL;
    }

    // list is empty
    // curr is NULL only when list is empty
    if(list->curr == NULL)
        return NULL;

    void* returnVal = list->curr->data;
    list->count--;
    nodes->count--;

    // list has single element
    if(list->first == list->last) {
        list->first->next = NULL;
        list->last->prev = NULL;
        list->curr = list->first = list->last = NULL;

        return returnVal;
    }

    // current is first
    if(list->curr == list->first) {
        list->curr = list->curr->next;
        list->first = list->curr->next;
        list->first->prev = NULL;

        return returnVal;
    }

    //current is last
    if(list->curr == list->last) {
        list->curr = list->last->prev;
        list->last = list->curr;
        list->curr->next = NULL;

        return returnVal;
    }

    //else
    Node* temp = list->curr;
    (temp->prev)->next = list->curr->next;
    (temp->next)->prev = list->curr->prev;
    list->curr = temp->next;

    return returnVal;
}


/*
 * adds list 2 to end of list 1
 * list 2 no longer exists
 */
void ListConcat(LIST* list1,LIST* list2) {
    if(list1->isFree==1 || list2->isFree==1)
    {
        printf("Free list detected\n");
        return;
    }

    if(list1==NULL && list2==NULL) {
        list2->isFree = 1;
        return;
    }

        // list 1 empty
    if(list1->curr == NULL) {
        list1->first = list2->first;
    }
        // list 2 empty
    else if(list2->curr == NULL) {
        list2->isFree = 1;
        return;
    }
    else {
        Node* temp = list1->last;
        temp->next = list2->first;
        (list2->first)->prev = temp;
    }

    list1->count += list2->count;
    list1->last = list2->last;
    list1->curr = list1->first;

    // remove list2
    list2->isFree = 1;
    list2->count = 0;
    list2->curr = list2->first = list2->last = NULL;

    return;
}


/*
 * return last item and take it out of the list
 * the new last item becomes the current item
 */
void *ListTrim(LIST* list) {
    if(list==NULL || list->isFree==1)
        return NULL;

    // Empty list
    if(list->first == NULL)
        return NULL;

    list->count--;
    nodes->count--;

    // Single Item list
    if(list->first == list->last) {
        return ListRemove(list);
    }

    // else
    void* returnVal = list->last->data;

    Node* temp = list->last;
    temp = temp->prev;
    list->curr = temp;
    list->curr->next = NULL;
    list->last->isFree = 1;
    list->last->prev = NULL;
    list->last = list->curr;

    return returnVal;
}


/*
 * delete list. itemFree is a pointer to a routine that frees
    an item.
 * invoked as (*itemFree)(itemToBeFreed);
 */
void ListFree(LIST* list, void (*itemFree)(Node*)) {
    if(list == NULL || list->isFree==1) {
        list->isFree = 1;
        return;
    }

    //empty list
    if(list->curr == NULL) {
        list->isFree = 1;
        return;
    }

    Node* top = list->first;

    while(top != NULL) {
        (*itemFree)(top);
        top = top->next;
    }
    list->isFree = 1;
    return;
}


/*
 * searches list starting at the current item until the end is reached
    or match is found.
 * Match is determined by the comparator parameter. This parameter
    is a pointer to a routine that takes as its first argument an
    item pointer, and as its second argument comparisonArg.
 * Comparator returns 0 if the item and comparisonArg don't match, 1 if they do.
 * Current pointer is left at the matched item and the pointer to that
    item is returned.
 * If no match, current pointer is left beyond end of list
    and a NULL pointer is returned.
 */
void *ListSearch(LIST* list, int (*comparator)(void*, void*), void* comparisonArg) {
    if(list == NULL || list->first == NULL || list->isFree==1) {
        return NULL;
    }

    //Starting at Current item
    Node* top = list->curr;
    int result = 0;
    while(top->next != NULL) {
        result = (*comparator)(top->data, comparisonArg);
        if(result == 1) {
            void* returnVal = top->data;
            list->curr = top;
            return returnVal;
        }
        top = top->next;
    }

    list->curr = list->last;
    return NULL;
}
