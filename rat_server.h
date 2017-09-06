#ifndef RAT_SERVER_H
#define RAT_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include "rat_http.h"
#include "rat_config.h"

#define NEVENTS 16

extern int start_server(rat_conf *conf);
extern int initialize_server(rat_conf *conf);

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
