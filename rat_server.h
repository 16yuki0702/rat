#ifndef RAT_SERVER_H
#define RAT_SERVER_H

#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include "rat_http.h"
#include "rat_config.h"
#include "rat_list.h"

#define NEVENTS 16

extern int start_server_http(rat_conf *conf);
extern int start_server_mqtt(rat_conf *conf);
extern void initialize_server(rat_conf *conf);

typedef struct {
	rat_conf *conf;
	int (*start_server)(rat_conf *conf);
} rat_server;

extern rat_server *r_server;

typedef struct {
	struct sockaddr_in s_addr;
	int s_sock;
	struct sockaddr_in addr;
	int sock;
	rat_conf *conf;
} rat_connection;

typedef struct {
	int efd;
	struct epoll_event e;
	struct epoll_event e_ret[NEVENTS];
} rat_event;

typedef struct {
	int sock;
	struct sockaddr_in addr;
	struct epoll_event e;
} r_connection;

typedef struct {
	int efd;
	struct epoll_event e;
	struct epoll_event e_ret[NEVENTS];
	int sock;
	struct sockaddr_in addr;
	rat_conf *conf;
} r_listener;

typedef struct {
	r_list *list;
	uint32_t c_count;
	r_listener listener;
} r_mqtt_manager;

#endif
