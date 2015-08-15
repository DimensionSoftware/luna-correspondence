#ifndef LinkedList_h
#define LinkedList_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

/* Very basic lisp recursive style linked list */
/* FIXME needs post/pre conditions and error handling */
/* FIXME add #define for starting index (0 or 1 most likely) */

/* interface */
typedef struct Node {
	void *_data;		/* node data */
	struct Node *_next;	/* next node */
} Node;

typedef struct LinkedList {
	Node *_head;	/* first node */
	Node *_tail;	/* last node */
	Node *_current;	/* holds current node */
	unsigned int _length;	/* list length */
	int _isMask;	/* set if list is a mask */
} LinkedList;

/* defines */
#define MAX_LINE_BUFFER 32


/* class methods */
LinkedList *LinkedList_alloc();
LinkedList *LinkedList_new();
LinkedList *LinkedList_listFromFile(char *fileName, void *(nodeBuilder)(void *));
LinkedList *LinkedList_maskFromList(LinkedList *, unsigned int, unsigned int);


/* instance methods */
LinkedList *LinkedList_init(LinkedList *);
LinkedList *LinkedList_initWithFile(LinkedList *, char *, void *(nodeBuilder)(void *));
LinkedList *LinkedList_initWithMask(LinkedList *, LinkedList *, unsigned int, unsigned int);
int LinkedList_dealloc(LinkedList *);
int LinkedList_delete(LinkedList *);
Node *LinkedList_nodeAtIndex(LinkedList *, unsigned int);
void *LinkedList_dataAtIndex(LinkedList *, unsigned int);
int LinkedList_push(LinkedList *, void *);
void *LinkedList_pop(LinkedList *);
int LinkedList_count(LinkedList *);


/* internal methods */
Node *__Node_new(void *data);
int __Node_dealloc(Node *);
int __Node_delete(Node *);


#ifdef __cplusplus
}
#endif

#endif /* LinkedList_h */
