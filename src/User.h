
#ifndef User_h
#define User_h

#ifdef __cplusplus
extern "C" {
#endif

#include "LinkedList.h"

typedef struct User {
	LinkedList *_userData;		/* linked list carrying user data */
	int *_entropyList;		/* list of random numbers */
	char *_userName;		/* user name */
} User;


/* defines */
#define selfProtocol    LinkedList_dataAtIndex(self->_userData, 1)
#define selfEncoding    LinkedList_dataAtIndex(self->_userData, 2)
#define selfSeed        LinkedList_dataAtIndex(self->_userData, 3)
#define selfRandomCount LinkedList_dataAtIndex(self->_userData, 4)

/* class methods */
User *User_alloc();
User *User_new(char *);

/* instance methods */
int User_sequenceNumberAtIndex(User *, int);
int User_init(User *, char *);
int User_dealloc(User *);
int User_delete(User *);
/* accessor methods */
char *User_userName(User *self);
LinkedList *User_userData(User *self);
int *User_entropyList(User *self);

/* private methods */
int __generateEntropyList(User *);

#ifdef __cplusplus
}
#endif

#endif /* User_h */
