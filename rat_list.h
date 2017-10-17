#ifndef RAT_LIST_H
#define RAT_LIST_H

typedef struct _list {
	void *data;
	struct _list *prev;
	struct _list *next;
} r_list;

#define LIST_INIT(list)						\
	do {							\
		list = (r_list*)malloc(sizeof(r_list));		\	
		list->data = NULL;				\
		list->prev = NULL;				\
		list->next = NULL;				\
	} while (0)

#define LIST_ENTRY(list, entry)					\
	do {							\
		r_list *tmplist = list;				\
		while (tmplist) {				\
			if (tmplist->data) {			\
				tmplist = tmplist->next;	\
			} else {				\
				tmplist->data = entry;		\
				break;				\
			}					\
		}						\
	} while (0)

#endif
