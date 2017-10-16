#ifndef RAT_LIST_H
#define RAT_LIST_H

extern r_list *root_list;

typedef struct _list {
	void *data;
	_list *prev;
	_list *next;
} r_list;

#define LIST_INIT (x)						\
	do {							\
		x->data = NULL;					\
		x->prev = NULL;					\
		x->next = NULL;					\
	} while (0)

#define LIST_ENTRY (x, d)					\
	do {							\
		r_list *tmplist = x;				\
		while (tmplist) {				\
			if (tmplist->data) {			\
				tmplist = tmlist->next;		\
			} else {				\
				tmplist->data = d;		\
				break;				\
			}					\
		}						\
	} while (0)

#endif
