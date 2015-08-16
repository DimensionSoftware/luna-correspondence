
#include "Luna.h"
#include "User.h"


/* ---------
   Luna Correspondence

	Copyleft 2002 Smurfboy <keith@dimensionsoftware.com>
     
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
	Public License as published by the Free Software Foundation; either version 2 of the License, or (at your
	option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
	for more details.

	You should have received a copy of the GNU General Public License along with this program; if not, write to
	the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


   ---------
   Feedback

	If you've found this source code useful, I'd love to hear about it.
	Please send an email to keith@dimensionsoftware.com

*/


/* begin class methods */
/* constructor */
User*
User_alloc()
{
	User *self = (User *)malloc(sizeof(User));
	#ifdef LUNA_DEBUG
	DEBUG("+ User");
	#endif
	return (self) ? self : NULL;
}

/* constructor w/ default initialize */
User*
User_new(char *userName)
{
	User *self = User_alloc();

	if (self) {
		if (User_init(self, userName)) {
			return self;
		} else {
			User_dealloc(self);
			return NULL;
		}
	} else {
		return NULL;
	}
}


/* begin instance methods */
int 
User_sequenceNumberAtIndex(User *self, int index)
{
	return self->_entropyList[index];
}

void*
__nodeParser(void *line)
{
	return line;
}

/* initializer */
int
User_init(User *self, char *userName)
{
	#define PATH_SIZE 7 + strlen(userName)
	char path[PATH_SIZE];

	if (!self) { return 0; }

	/* apply defaults */
	self->_userName = userName;
	snprintf(path, PATH_SIZE, "users/%s", userName);
	if ((self->_userData = LinkedList_listFromFile(path, __nodeParser)) == NULL) {
		return 0;
	}
	__generateEntropyList(self);
	return 1;
}

/* destructor */
int
User_dealloc(User *self)
{
	if (!self) return 0;
	free(self);
	self = NULL;

	#ifdef LUNA_DEBUG
	DEBUG("- User");
	#endif
	return 1;
}

/* destructor for initialization values too */
int
User_delete(User *self)
{
	if (!self) { return 0; }
	LinkedList_delete(self->_userData);
	return User_dealloc(self);
}

/* accessor methods */
char*
User_userName(User *self)
{
	if (!self) { return NULL; }
	return self->_userName;
}

LinkedList*
User_userData(User *self)
{
	if (!self) { return NULL; }
	return self->_userData;
}

int*
User_entropyList(User *self)
{
	if (!self) { return NULL; }
	return self->_entropyList;
}

/* begin private methods */
int
__generateEntropyList(User *self)
{
	int i;

	self->_entropyList = malloc(sizeof(int) * atoi(selfRandomCount));
	if (!self->_entropyList) { return 0; }

	for(i = 0; i < atoi(selfRandomCount); ++i) {
		self->_entropyList[i] = rand() % 2147483647 + 1;
	}
	return 1;
}
