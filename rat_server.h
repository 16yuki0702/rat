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
#include <errno.h>
#include <stdbool.h>
#include "rat_config.h"
#include "rat_string.h"
#include "rat_net.h"
#include "rat_mqtt.h"

#define NEVENTS			16
#define BUF_SIZE		1024

#define LISTENER_SERVER		0x01
#define LISTENER_CLUSTER	0x02

#define CONNECTION_SERVER	0x01
#define CONNECTION_CLUSTER	0x02

#define ADD_EVENT(l, entry)								\
	do {										\
		if (epoll_ctl(l->efd, EPOLL_CTL_ADD, entry->sock, &entry->e) != 0) {	\
			perror("epoll_ctl");						\
			exit(-1);							\
		}									\
	} while (0)	
		
#define MODIFY_EVENT(l, entry, flags)							\
	do {										\
		entry->e.events = flags;						\
		if (epoll_ctl(l->efd, EPOLL_CTL_MOD, entry->sock, &entry->e) != 0) {	\
			perror("epoll_ctl");						\
			exit(-1);							\
		}									\
	} while (0)	

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
	int efd;
	struct epoll_event e;
	struct epoll_event e_ret[NEVENTS];
	int sock;
	struct sockaddr_in addr;
	rat_conf *conf;
	uint8_t type;
} r_listener;

extern int start_server_http(rat_conf *conf);
extern int start_server_mqtt(rat_conf *conf);
extern void initialize_server(rat_conf *conf);
extern buf* read_socket(int sock);

#endif
