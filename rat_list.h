#ifndef RAT_LIST_H
#define RAT_LIST_H

#include <stdlib.h>
#include <stdint.h>

typedef struct _list {
	void *data;
	struct _list *prev;
	struct _list *next;
} r_list;

#define LIST_INIT(list)											\
	do {												\
		list = (r_list*)malloc(sizeof(r_list));							\
		list->data = NULL;									\
		list->prev = NULL;									\
		list->next = NULL;									\
	} while (0)

#define LIST_ADD(list, entry)										\
	do {												\
		r_list *e = list;									\
		while (e) {										\
			if (e->data) {									\
				if (e->next) {								\
					e = e->next;							\
				} else {								\
					e->next = (r_list*)malloc(sizeof(r_list));			\
					e->next->data = entry;						\
					e->next->prev = e;						\
					break;								\
				}									\
			} else {									\
				e->data = entry;							\
				break;									\
			}										\
		}											\
	} while (0)

#define LIST_FIND(entry, list, data, type)								\
	do {												\
		r_list *e = list;									\
		while (e) {										\
			if (*(type*)e->data == data) {							\
				break;									\
			} else {									\
				if (e->next) {								\
					e = e->next;							\
				} else {								\
					e = NULL;							\
					break;								\
				}									\
			}										\
		}											\
		entry = e->data;									\
	} while (0)

#define LIST_DUMP(list, type)										\
	do {												\
		r_list *e = list;									\
		while (e) {										\
			if (e->data) {									\
				printf("list data %d\n", *(type*)e->data);				\
			}										\
			e = e->next;									\
		}											\
	} while (0)

#endif
