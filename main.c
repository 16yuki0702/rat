#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "http.h"

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
	int error_code = 0;

	if (argv[1] == NULL) {
		printf("please specify config file.\n");
		return -1;
	}

	conf_path = argv[1];
	error_code = read_config(conf_path);

	if (error_code) {
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
