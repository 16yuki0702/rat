#ifndef RAT_LIST_H
#define RAT_LIST_H

typedef struct _list {
	void *data;
	_list *prev;
	_list *next;
} r_list;

#define LIST_INIT (x)		\
	do {			\
		x->data = NULL;	\
		x->prev = NULL;	\
		x->next = NULL;	\
	} while (0)

#endif
