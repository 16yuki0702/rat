#ifndef RAT_BACKEND_H
#define RAT_BACKEND_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <hiredis.h>
#include "rat_mqtt.h"

typedef struct _topic_entry {
	uint8_t qos;
	uint32_t sock;
} topic_entry;

#define SCAN_SUBSCRIPTION(value, t)						\
	do {									\
		sprintf(value, "%d,%d", t->qos, t->sock);			\
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
		t.qos = atoi(strtok(value, s));					\
		t.sock = atoi(strtok(NULL, s));					\
	} while (0)

#define DUMP_TOPIC(t)								\
	do {									\
		printf("qos : %d\n", t.qos);					\
		printf("sock : %d\n", t.sock);					\
	} while (0)

typedef struct {
	r_str topic;
	int sock;
	uint8_t qos;
} subscription;

extern int store_data(subscription *sub, rat_conf *conf);
extern int publish_data(r_str *topic, uint8_t *data, uint32_t len, rat_conf *conf);

#endif
