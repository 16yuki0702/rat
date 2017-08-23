#ifndef RAT_SERVER_H
#define RAT_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include "http.h"
#include "rat_config.h"

#define NEVENTS 16

extern int open_socket(rat_conf *conf);

typedef struct {
	struct sockaddr_in addr;
	int sock;
} rat_connection;

typedef struct {
	int efd;
	struct epoll_event e;
	struct epoll_event e_ret[NEVENTS];
} rat_event;

#endif
