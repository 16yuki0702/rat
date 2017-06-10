#ifndef RAT_SERVER_H
#define RAT_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include "http.h"
#include "config.h"

extern int open_socket(rat_conf *conf);

#endif