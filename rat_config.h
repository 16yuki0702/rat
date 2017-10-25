#ifndef RAT_CONFIG_H
#define RAT_CONFIG_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "rat_string.h"
#include "rat_log.h"
#include "rat_list.h"

typedef struct {
	uint16_t port;
	rat_str *host;
	rat_str *protocol;
	uint16_t backlog;
	int socket_reuse;
	int log_level;
	rat_str *redis_server;
	int redis_port;
	uint16_t cluster_enable;
	r_list *clusters;
} rat_conf;

typedef struct {
	int r_code;
	char *r_message;
} config_detail;

extern rat_conf *conf;

extern int read_config(char *path);

#endif
