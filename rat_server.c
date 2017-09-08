#include "rat_server.h"
#include "rat_http.h"

http_request *rat_request;

static void
_send_response(int c_socket)
{
	char filepath[1024], file_buffer[1024];
	FILE *fp;

	if (is_entry_same(rat_request->uri.p, "/", rat_request->uri.len)) {
		RAT_STR_CPY(filepath, "index.html");
	} else {
		GET_ENTRY_URI(filepath, rat_request->uri);
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
_create_connection(rat_conf *conf)
{
	rat_connection *conn;
	conn = (rat_connection*)malloc(sizeof(rat_connection));

	conn->s_sock = socket(AF_INET, SOCK_STREAM, 0);
	conn->s_addr.sin_family = AF_INET;
	conn->s_addr.sin_port = htons(conf->port);
	conn->s_addr.sin_addr.s_addr = INADDR_ANY;
	conn->conf = conf;

	return conn;
}

static rat_event*
_create_event(int sock)
{
	rat_event *event;
	event = (rat_event*)malloc(sizeof(rat_event));

	if ((event->efd = epoll_create(NEVENTS)) < 0) {
		LOG_ERROR(("epoll_create"));
		return event;
	}

	memset(&event->e, 0, sizeof(event->e));	
	event->e.events = EPOLLIN;
	event->e.data.fd = sock;
	if (epoll_ctl(event->efd, EPOLL_CTL_ADD, sock, &event->e)) {
		LOG_ERROR(("epoll_ctl"));
		return event;
	}

	return event;
}

static int
_server_loop(rat_connection *conn)
{
	int on = 1;
	rat_event *e;
	e = _create_event(conn->s_sock);
	ioctl(conn->s_sock, FIONBIO, &on);
	fcntl(conn->s_sock, F_SETFD, FD_CLOEXEC);

	char read_buffer[1024];
	int i, nfds, c_len;

	while (1) {
		if ((nfds = epoll_wait(e->efd, e->e_ret, NEVENTS, -1)) <= 0) {
			LOG_ERROR(("epoll_wait"));
			return 1;
		}
		for (i = 0; i < nfds; i++) {

			if (e->e_ret[i].data.fd != e->e.data.fd) {
				continue;
			}

			memset(read_buffer, 0, sizeof(read_buffer));

			c_len = sizeof(conn->addr);
			if ((conn->sock = accept(conn->s_sock, (struct sockaddr *)&conn->addr, &c_len)) == -1) {
				LOG_ERROR(("failed open socket."));
				return -1;
			}

			if (read(conn->sock, read_buffer, sizeof(read_buffer)) == -1) {
				LOG_ERROR(("failed read socket."));
				return -1;
			}

			http_request_parse(read_buffer);

			_send_response(conn->sock);
		}
	}

	close(conn->s_sock);

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
	rat_connection *conn;
	conn = _create_connection(conf);

	if (conn->conf->socket_reuse) {
		setsockopt(conn->s_sock, SOL_SOCKET, SO_REUSEADDR, &conn->conf->socket_reuse, sizeof(conn->conf->socket_reuse));
	}

	if (bind(conn->s_sock, (struct sockaddr *)&conn->s_addr, sizeof(conn->s_addr)) != 0) {
		LOG_ERROR(("failed bind socket."));
		return -1;
	}

	if (listen(conn->s_sock, conn->conf->backlog) != 0) {
		LOG_ERROR(("failed listen socket."));
		return -1;
	}

	return _server_loop(conn);
}
