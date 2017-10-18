#include "rat_server.h"
#include "rat_http.h"
#include "rat_mqtt.h"

http_request *rat_request;
rat_server *r_server;

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
	int on = 1;
	rat_connection *conn;

	conn = (rat_connection*)malloc(sizeof(rat_connection));

	conn->s_sock = socket(AF_INET, SOCK_STREAM, 0);
	conn->s_addr.sin_family = AF_INET;
	conn->s_addr.sin_port = htons(conf->port);
	conn->s_addr.sin_addr.s_addr = INADDR_ANY;
	ioctl(conn->s_sock, FIONBIO, &on);
	fcntl(conn->s_sock, F_SETFD, FD_CLOEXEC);

	if (conf->socket_reuse) {
		setsockopt(conn->s_sock, SOL_SOCKET, SO_REUSEADDR, &conf->socket_reuse, sizeof(conf->socket_reuse));
	}

	if (bind(conn->s_sock, (struct sockaddr *)&conn->s_addr, sizeof(conn->s_addr)) != 0) {
		LOG_ERROR(("failed bind socket."));
		close(conn->s_sock);
		free(conn);
		return NULL;
	}

	if (listen(conn->s_sock, conf->backlog) != 0) {
		LOG_ERROR(("failed listen socket."));
		close(conn->s_sock);
		free(conn);
		return NULL;
	}

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

static r_listener*
_create_listener(rat_conf *conf)
{
	int on = 1;
	r_listener *l;

	l = (r_listener*)malloc(sizeof(r_listener));

	l->sock = socket(AF_INET, SOCK_STREAM, 0);
	l->addr.sin_family = AF_INET;
	l->addr.sin_port = htons(conf->port);
	l->addr.sin_addr.s_addr = INADDR_ANY;
	ioctl(l->sock, FIONBIO, &on);
	fcntl(l->sock, F_SETFD, FD_CLOEXEC);

	if (conf->socket_reuse) {
		setsockopt(l->sock, SOL_SOCKET, SO_REUSEADDR, &conf->socket_reuse, sizeof(conf->socket_reuse));
	}

	if (bind(l->sock, (struct sockaddr *)&l->addr, sizeof(l->addr)) != 0) {
		LOG_ERROR(("failed bind socket."));
		close(l->sock);
		free(l);
		return NULL;
	}

	if (listen(l->sock, conf->backlog) != 0) {
		LOG_ERROR(("failed listen socket."));
		close(l->sock);
		free(l);
		return NULL;
	}

	if ((l->efd = epoll_create(NEVENTS)) < 0) {
		LOG_ERROR(("epoll_create"));
		return l;
	}

	memset(&l->e, 0, sizeof(l->e));	
	l->e.events = EPOLLIN;
	l->e.data.fd = l->sock;
	if (epoll_ctl(l->efd, EPOLL_CTL_ADD, l->sock, &l->e)) {
		LOG_ERROR(("epoll_ctl"));
		return NULL;
	}

	l->conf = conf;

	return l;
}

static r_connection *
_create_new_connection(int sock)
{
	r_connection *c;
	c = (r_connection*)malloc(sizeof(r_connection));
	memset(c, 0, sizeof(c));

	c->sock = sock;
	c->e.events = EPOLLIN | EPOLLOUT;
	c->e.data.fd = sock;

	return c;
}

static int
_server_loop(rat_connection *conn)
{
	rat_event *e;
	e = _create_event(conn->s_sock);

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

			memset(&e->e, 0, sizeof(e->e));
			e->e.events = EPOLLIN | EPOLLONESHOT;

			if (epoll_ctl(e->efd, EPOLL_CTL_ADD, conn->sock, &e->e) != 0) {
				 perror("epoll_ctl");
				 return 1;
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

static int
_server_loop_mqtt(r_listener *l)
{
	r_connection *entry;
	r_mqtt_manager *mng;
	char read_buffer[1024];
	int i, nfds, c_len, client;

	mng = (r_mqtt_manager*)malloc(sizeof(r_mqtt_manager));
	memset(mng, 0, sizeof(r_mqtt_manager));

	while (1) {
		if ((nfds = epoll_wait(l->efd, l->e_ret, NEVENTS, -1)) <= 0) {
			LOG_ERROR(("epoll_wait"));
			return -1;
		}
		for (i = 0; i < nfds; i++) {

			if (l->e_ret[i].data.fd == l->e.data.fd) {
				memset(read_buffer, 0, sizeof(read_buffer));
				c_len = sizeof(l->addr);
				if ((client = accept(l->sock, (struct sockaddr *)&l->addr, &c_len)) == -1) {
					LOG_ERROR(("failed open socket."));
					return -1;
				}

				entry = _create_new_connection(client);
				mng->c_count++;
				LIST_INIT(mng->list);
				LIST_ADD(mng->list, entry);
				LIST_DUMP(mng->list);

				if (epoll_ctl(l->efd, EPOLL_CTL_ADD, client, &entry->e) != 0) {
					perror("epoll_ctl");
					return -1;
				}

			} else {
				parse_mqtt(entry);
			}
		}
	}

	close(l->sock);

	free(l);

	return 0;
}

int
initialize_server(rat_conf *conf)
{ 
	r_server = (rat_server*)malloc(sizeof(rat_server));

	if (!(strcmp(conf->protocol->data, "http"))) {
		r_server->start_server = start_server_http;
	} else if (!(strcmp(conf->protocol->data, "mqtt"))) {
		r_server->start_server = start_server_mqtt;
	}

	r_server->conf = conf;
}

int
start_server_http(rat_conf *conf)
{
	rat_connection *conn;
	conn = _create_connection(conf);

	return _server_loop(conn);
}

int
start_server_mqtt(rat_conf *conf)
{
	r_listener *l;
	l = _create_listener(conf);

	return _server_loop_mqtt(l);
}
