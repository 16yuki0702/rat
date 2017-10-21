#ifndef RAT_BACKEND_H
#define RAT_BACKEND_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <hiredis.h>

typedef struct _topic_entry {
	uint16_t message_id;
	uint8_t qos;
	uint32_t sock;
} topic_entry;

topic_entry ts[3] = {
	{1, 1, 7},
	{2, 0, 8},
	{3, 1, 9}
};

#define SCAN_TOPIC(value, t)							\
	do {									\
		sprintf(value, "%d,%d,%d", t.message_id, t.qos, t.sock);	\
	} while (0)

#define PRINT_TOPIC(value)							\
	do {									\
		char *w = NULL, *s = ",";					\
		for (w = strtok(value, s); w; w = strtok(NULL, s)) {		\
			printf("%d\n", atoi(w));				\
		}								\
	} while (0)

#define PARSE_TOPIC(t, value)							\
	do {									\
		char *s = ",";							\
		t.message_id = atoi(strtok(value, s));				\
		t.qos = atoi(strtok(NULL, s));					\
		t.sock = atoi(strtok(NULL, s));					\
	} while (0)

#define DUMP_TOPIC(t)								\
	do {									\
		printf("message_id : %d\n", t.message_id);			\
		printf("qos : %d\n", t.qos);					\
		printf("sock : %d\n", t.sock);					\
	} while (0)

extern store_data(char *topic);

#endif
