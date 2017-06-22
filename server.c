#include "server.h"
#include "http.h"

http_request *rat_request;

static int
_open_socket(rat_conf *conf)
{
	int server_socket, client_socket;
	int client_len;
	int read_size;
	struct sockaddr_in server_addr, client_addr;
	const int yes = 1;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(conf->port);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	if (conf->socket_reuse) {
		setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &conf->socket_reuse, sizeof(conf->socket_reuse));
	}

	if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
		printf("failed bind socket.\n");
		return -1;
	}

	if (listen(server_socket, conf->backlog) != 0) {
		printf("failed listen socket.\n");
		return -1;
	}

	if (conf->non_blocking) {
		ioctl(server_socket, FIONBIO, &conf->non_blocking);
	}
	
	while (1) {
		char read_buffer[1024];
		memset(read_buffer, 0, sizeof(read_buffer));

		client_len = sizeof(client_addr);
		client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
		if (client_socket == -1) {
			printf("failed open socket.\n");
			return -1;
		}

		read_size = read(client_socket, read_buffer, sizeof(read_buffer));
		if (read_size == -1) {
			printf("failed read socket.\n");
			return -1;
		}
		printf("%s\n", read_buffer);

		write(client_socket, HTTP_200_RES, strlen(HTTP_200_RES));

		close(client_socket);
	}

	close(server_socket);

	return 0;
}

#define NEVENTS 16

static void
_send_response(int c_socket)
{
	char *filepath;

	if (!strcmp(rat_request->uri, "/")) {
		filepath = "index.html";
	} else {
		filepath = rat_request->uri;
	}
	
	char file_buffer[1024];
	FILE *fp;

	fp = fopen(filepath, "r");
	if (fp == NULL) {
		write(c_socket, HTTP_404_RES, strlen(HTTP_404_RES));
		close(c_socket);
		return;
	}

	write(c_socket, HTTP_200_RES, strlen(HTTP_200_RES));
	while (fgets(file_buffer, 1024, fp)) {
		write(c_socket, file_buffer, strlen(file_buffer));
	}

	close(c_socket);
}

static int
_epoll_loop(int s_socket)
{
 	int epfd;
 	struct epoll_event ev, ev_ret[NEVENTS];

	epfd = epoll_create(NEVENTS);
	if (epfd < 0) {
		perror("epoll_create");
		return 1;
	}

	memset(&ev, 0, sizeof(ev));
	ev.events = EPOLLIN;
	ev.data.fd = s_socket;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, s_socket, &ev) != 0) {
		perror("epoll_ctl");
		return 1;
	}

	int flg = 0;
	int i, n, nfds;
	struct sockaddr_in c_addr;
	int c_socket, c_len, read_size;

	while (1) {
		nfds = epoll_wait(epfd, ev_ret, NEVENTS, -1);
		if (nfds <= 0) {
			perror("epoll_wait");
			return 1;
		}
		for (i = 0; i < nfds; i++) {

			if (ev_ret[i].data.fd != s_socket) {
				continue;
			}

			char read_buffer[1024];
			memset(read_buffer, 0, sizeof(read_buffer));

			c_len = sizeof(c_addr);
			c_socket = accept(s_socket, (struct sockaddr *)&c_addr, &c_len);
			if (c_socket == -1) {
				printf("failed open socket.\n");
				return -1;
			}

			read_size = read(c_socket, read_buffer, sizeof(read_buffer));
			if (read_size == -1) {
				printf("failed read socket.\n");
				return -1;
			}
			http_request_parse(read_buffer);

			_send_response(c_socket);
		}
	}

LOOP_END:

	close(s_socket);

	return 0;
}

static int
_open_socket_epoll(rat_conf *conf)
{
	int s_socket;
	struct sockaddr_in s_addr;

	s_socket = socket(AF_INET, SOCK_STREAM, 0);
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(conf->port);
	s_addr.sin_addr.s_addr = INADDR_ANY;
	if (conf->socket_reuse) {
		setsockopt(s_socket, SOL_SOCKET, SO_REUSEADDR, &conf->socket_reuse, sizeof(conf->socket_reuse));
	}
	if (bind(s_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) != 0) {
		printf("failed bind socket.\n");
		return -1;
	}

	if (listen(s_socket, conf->backlog) != 0) {
		printf("failed listen socket.\n");
		return -1;
	}

	if (conf->non_blocking) {
		ioctl(s_socket, FIONBIO, &conf->non_blocking);
	}

	return _epoll_loop(s_socket);
}

int
open_socket(rat_conf *conf)
{
	if (conf->use_epoll) {
		_open_socket_epoll(conf);
	} else {
		_open_socket(conf);
	}
}
