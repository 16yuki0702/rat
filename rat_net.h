#ifndef RAT_NET_H
#define RAT_NET_H

#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include "rat_config.h"

#define BUF_SIZE			1024

typedef struct {
	uint8_t *d;
	int len;
} buf;

typedef struct _cluster_node {
	int sock;
	uint8_t myself;
	rat_str *name;
	int port;
} cluster_node;

typedef struct {
	cluster_node **nodes;
	int count;
} cluster_list;

typedef struct {
	uint8_t cmd:4;
	uint8_t dup:1;
	uint8_t qos:2;
	uint8_t retain:1;
	uint32_t remain;
	r_str protocol_name;
	uint8_t protocol_version;
	uint8_t connect_flags;
	uint16_t keepalive_timer;
	r_str client_id;
	r_str will_topic;
	r_str will_message;
	r_str username;
	r_str password;
	uint16_t message_id;
	r_str payload;
	r_str topic;
} r_mqtt_packet;

typedef struct _r_connection {
	int sock;
	struct sockaddr_in addr;
	struct epoll_event e;
	rat_conf *conf;
	buf *b;
	r_mqtt_packet *p;
	cluster_list *clusters;
	r_list *request_queue;
	uint8_t type;
	void (*handle_mqtt)(struct _r_connection *c);
} r_connection;

extern void free_buf(buf *b);

#endif
