
#include "LinkedList.h"
#include "Luna.h"

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
LinkedList*
LinkedList_alloc()
{
	LinkedList *self = (LinkedList *)malloc(sizeof(LinkedList));
	#ifdef LUNA_DEBUG
	DEBUG("+ LinkedList");
	#endif
	return (self) ? self : NULL;
}

/* constructor w/ default initialize */
LinkedList*
LinkedList_new()
{
	LinkedList *self = LinkedList_alloc();
	return (self)
		? LinkedList_init(self)
		: NULL;
}


/* constructor w/ list from file */
LinkedList*
LinkedList_listFromFile(char *fileName, void *(nodeBuilder)(void *))
{
	LinkedList *self = LinkedList_alloc();
	return (self)
		? LinkedList_initWithFile(self, fileName, nodeBuilder)
		: NULL;
}

/* constructor for a linked list mask */
LinkedList*
LinkedList_maskFromList(LinkedList *aLinkedList, unsigned int offset, unsigned int length)
{
	LinkedList *self;

	if (!aLinkedList) return 0;

	self = LinkedList_alloc();
	return (self)
		? LinkedList_initWithMask(self, aLinkedList, offset, length)
		: NULL;
}
/* end class methods */


/* begin instance methods */
/* default initializer */
LinkedList*
LinkedList_init(LinkedList *self)
{
	if (!self) { return 0; }
	self->_head   = 0;
	self->_tail   = 0;
	self->_length = 0;
	self->_isMask = 0;
	return self;
}

/* initialize from file */
LinkedList*
LinkedList_initWithFile(LinkedList *self, char *fileName, void *(nodeBuilder)(void *))
{
	FILE *fileHandle;
	size_t lineLength;
	char *line;
	void *nodeData;

	if (!self) { return 0; }

	/* initialize defaults */
	LinkedList_init(self);

	/* populate list economy style */
	if ((fileHandle = fopen(fileName, "r")) == NULL) { return 0; }
	while ((line = fgetln(fileHandle, &lineLength)) != NULL) {
		if (line[0] == '#' || line[0] == '\n') { continue; }

		if (lineLength > MAX_LINE_BUFFER - 1) {
			nodeData = malloc(MAX_LINE_BUFFER);
			strncpy(nodeData, line, MAX_LINE_BUFFER - 1);
			strncpy(nodeData + MAX_LINE_BUFFER, "\0", 1);
		} else {
			nodeData = malloc(lineLength);
			strncpy(nodeData, line, lineLength - 1);
			strncpy(nodeData + lineLength, "\0", 1);
		}
		LinkedList_push(self, nodeBuilder(nodeData));
	}
	fclose(fileHandle);
	return self;
}

LinkedList *
LinkedList_initWithMask(LinkedList *self, LinkedList *aLinkedList, unsigned int offset, unsigned int length)
{
	int ndx;

	if (!self) { return NULL; }
	if (!aLinkedList || !aLinkedList->_length) { return NULL; }

	self->_isMask = 1;

	/* set mask head */
	self->_current = aLinkedList->_head;
	if (offset <= aLinkedList->_length) {
		for (ndx = 1; ndx < offset; ++ndx) {
			self->_current = self->_current->_next;
		}
		self->_head = self->_current;
	} else {
		return 0;
	}

	/* set mask tail */
	if (length) {
		for (ndx = 1; ndx <= length; ++ndx) {
			self->_current = self->_current->_next;
		}
		self->_tail = self->_current;
	} else {
		return 0;
	}

	/* set mask length */
	self->_length = length;

	return self;
}

/* destructor */
int
LinkedList_dealloc(LinkedList *self)
{
	if (!self) return 0;
	free(self);
	self->_length = 0;
	self          = NULL;

	#ifdef LUNA_DEBUG
	DEBUG("- LinkedList");
	#endif
	return 1;
}

/* destructor for initialization values too */
int
LinkedList_delete(LinkedList *self)
{
	Node *tmp;

	if (!self) { return 0; }
	if (self->_isMask) { return 0; }

	self->_current = self->_head;
	while (self->_current) {
		tmp            = self->_current;
		self->_current = self->_current->_next;
		__Node_delete(tmp);
	}
	return LinkedList_dealloc(self);
}

/* return Node at index (starting at 1) */
Node*
LinkedList_nodeAtIndex(LinkedList *self, unsigned int index)
{
	int ndx;

	if (!self || !self->_length) { return 0; }
	self->_current = self->_head;
	if (index <= self->_length) {
		for (ndx = 1; ndx < index; ++ndx) {
			self->_current = self->_current->_next;
		}
		return self->_current;
	}
	return NULL;
}

/* return data at index */
void*
LinkedList_dataAtIndex(LinkedList *self, unsigned int index)
{
	Node *aNode;

	if (!self || !self->_length) { return 0; }
	aNode = LinkedList_nodeAtIndex(self, index);
	if (aNode) { return aNode->_data; }
	return 0;
}


/* append to list */
int
LinkedList_push(LinkedList *self, void *data)
{
	Node *aNode = __Node_new(data);

	if (!self) { return 0; }
	if (self->_isMask) { return 0; }

	if (self->_length == 0 && aNode) {
		self->_tail = self->_head = aNode;
		self->_length = 1;
	} else if (aNode) {
		self->_tail->_next = aNode;
		self->_tail = aNode;
		++self->_length;
	}
	return self->_length;
}

/* free()'s Node, but not data pointer */
void*
LinkedList_pop(LinkedList *self)
{
	void *data = NULL;

	if (!self || !self->_length) { return 0; }
	if (self->_isMask) { return 0; }

	data = self->_tail->_data;
	__Node_dealloc(self->_tail);

	if (self->_length > 1) {
		self->_tail        = LinkedList_nodeAtIndex(self, self->_length - 1);
		self->_tail->_next = NULL;
	} else {
		self->_tail = self->_head = NULL;
	}
	--self->_length;
	return data;
}

int LinkedList_count(LinkedList *self)
{
	if (!self) { return 0; }
	return self->_length;
};
/* end instance methods */


/* begin internal methods */
/* returns a preinitialized Node */
Node*
__Node_new(void *data)
{
	Node *aNode = (Node *)malloc(sizeof(Node));
	if (aNode) {
		aNode->_data = data;
		aNode->_next = NULL;
		return aNode;
	}
	return NULL;
}

/* free()'s a Node */
int
__Node_dealloc(Node *aNode)
{
	if (aNode) {
		free(aNode);
		aNode = NULL;
		return 1;
	}
	return 0;
}

/* free()'s Node and data pointer */
int
__Node_delete(Node *aNode)
{
	if (aNode) {
		free(aNode->_data);
		free(aNode);
		aNode = NULL;
		return 1;
	}
	return 0;
}
/* end internal methods */
