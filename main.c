#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "http.h"

static int conf_error = 0;

typedef struct {
	uint16_t port;
	char *host;
	char *protocol;
	uint16_t backlog;
	int socket_reuse;
} rat_conf;

void
conf_handler_string(char **conf, const char *param)
{
	char *ret;
	int len;

	ret = strdup(param);
	len = strlen(ret);

	if (ret[len - 1] == '\n') {
		ret[len - 1] = '\0';
	}

	*conf = ret;
}

void
conf_handler_int(int *conf, const char *param)
{
	int ret;

	ret = atoi(param);

	*conf = ret;
}

void
conf_handler_uint16(uint16_t *conf, const char *param)
{
	uint16_t ret;

	ret = atoi(param);

	*conf = ret;
}

rat_conf *
read_config(char *path)
{
	FILE *f;
	char *str;
	char buf[1024];
	char *token;
	char *cptr;

	rat_conf *conf;
	conf = (rat_conf *)malloc(sizeof(rat_conf));

	f = fopen(path, "rt");
	while (fgets(buf, 1024, f)) {
		token = strtok_r(buf, " ", &cptr);
		if (!strcmp(token, "#")) {
			// skip comment section.
		} else if (!strcmp(token, "host")) {
			conf_handler_string(&conf->host, strtok_r(NULL, "", &cptr));
		} else if (!strcmp(token, "port")) {
			conf_handler_uint16(&conf->port, strtok_r(NULL, "", &cptr));
		} else if (!strcmp(token, "protocol")) {
			conf_handler_string(&conf->protocol, strtok_r(NULL, "", &cptr));
		} else if (!strcmp(token, "backlog")) {
			conf_handler_uint16(&conf->backlog, strtok_r(NULL, "", &cptr));
		} else if (!strcmp(token, "socket_reuse")) {
			conf_handler_int(&conf->socket_reuse, strtok_r(NULL, "", &cptr));
		}
	}

	return conf;
}

int
open_socket(rat_conf *conf)
{
	int server_socket, client_socket;
	int server_len, client_len;
	int read_size;
	struct sockaddr_in server_addr, client_addr;
	const int yes = 1;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(conf->port);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	if (conf->socket_reuse == 1) {
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

int
main(int argc, char *argv[])
{
	char *conf_path;
	rat_conf *conf;

	if (argv[1] == NULL) {
		printf("please specify config file.\n");
		return -1;
	}

	conf_path = argv[1];
	conf = read_config(conf_path);

	if (conf_error) {
		printf("please review config file. there is error config.\n");
	}

	open_socket(conf);
	
	printf("%s\n", conf->host);
	printf("%d\n", conf->port);
	printf("%s\n", conf->protocol);
	printf("%d\n", conf->backlog);
	printf("%d\n", conf->socket_reuse);

	return 0;
}
