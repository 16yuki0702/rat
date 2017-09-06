#include "server.h"
#include "http.h"

http_request *rat_request;

static void
_send_response(int c_socket)
{
	char *filepath;
	char file_buffer[1024];
	FILE *fp;

	if (!strcmp(rat_request->uri->data, "/")) {
		filepath = "index.html";
	} else {
		filepath = rat_request->uri->data;
	}

	if ((fp = fopen(filepath, "r")) == NULL) {
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

static rat_connection*
_create_connection()
{
	rat_connection *conn;
	conn = (rat_connection*)malloc(sizeof(rat_connection));

	return conn;
}

static rat_event*
_create_event(int sock)
{
	rat_event *event;
	event = (rat_event*)malloc(sizeof(rat_event));

	if ((event->efd = epoll_create(NEVENTS)) < 0) {
		perror("epoll_create");
		return event;
	}

	memset(&event->e, 0, sizeof(event->e));	
	event->e.events = EPOLLIN;
	event->e.data.fd = sock;
	if (epoll_ctl(event->efd, EPOLL_CTL_ADD, sock, &event->e)) {
		perror("epoll_ctl");
		return event;
	}

	return event;
}

static int
_server_loop(int s_socket, rat_conf *conf)
{
	int on = 1;
	rat_event *e;
	e = _create_event(s_socket);
	ioctl(s_socket, FIONBIO, &on);

	char read_buffer[1024];
	int i, nfds, c_len;
	rat_connection *conn;
	conn = _create_connection();

	while (1) {
		if ((nfds = epoll_wait(e->efd, e->e_ret, NEVENTS, -1)) <= 0) {
			perror("epoll_wait");
			return 1;
		}
		for (i = 0; i < nfds; i++) {

			if (e->e_ret[i].data.fd != e->e.data.fd) {
				continue;
			}

			memset(read_buffer, 0, sizeof(read_buffer));

			c_len = sizeof(conn->addr);
			if ((conn->sock = accept(s_socket, (struct sockaddr *)&conn->addr, &c_len)) == -1) {
				_ERROR(("failed open socket."));
				return -1;
			}

			if (read(conn->sock, read_buffer, sizeof(read_buffer)) == -1) {
				_ERROR(("failed read socket."));
				return -1;
			}

			http_request_parse2(read_buffer);

			_send_response(conn->sock);
		}
	}

	close(s_socket);

	free(conn);

	return 0;
}

int
initialize_server(rat_conf *conf)
{ 
	(void*)conf;
}

int
start_server(rat_conf *conf)
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
		_ERROR(("failed bind socket."));
		return -1;
	}

	if (listen(s_socket, conf->backlog) != 0) {
		_ERROR(("failed listen socket."));
		return -1;
	}

	return _server_loop(s_socket, conf);
}
