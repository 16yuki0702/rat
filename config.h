#ifndef RAT_CONFIG_H
#define RAT_CONFIG_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "rat_string.h"

typedef struct {
	uint16_t port;
	char *host;
	char *protocol;
	uint16_t backlog;
	int socket_reuse;
	int use_epoll;
	int log_level;
} rat_conf;

typedef struct {
	int r_code;
	char *r_message;
} config_detail;

extern rat_conf *conf;

extern int read_config(char *path);

#endif
