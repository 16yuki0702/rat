#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct {
	uint16_t port;
	char *host;
	char *protocol;
	uint16_t backlog;
} rat_conf;

char *
conf_handler_string(const char *param)
{
	char *ret;
        int len;

	ret = strdup(param);
	len = strlen(ret);

	if (ret[len - 1] == '\n') {
		ret[len - 1] = '\0';
	}

	return ret;
}

int
conf_handler_int(const char *param)
{
	int ret;

	ret = atoi(param);

	return ret;
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
	while(fgets(buf, 1024, f)) {
		token = strtok_r(buf, " ", &cptr);
		if (!strcmp(token, "#")) {
			printf("this is comment.\n");
			continue;
		}
		if (!strcmp(token, "host")) {
			conf->host = conf_handler_string(strtok_r(NULL, "", &cptr));
			continue;
		}
		if (!strcmp(token, "port")) {
			conf->port = conf_handler_int(strtok_r(NULL, "", &cptr));
			continue;
		}
                if (!strcmp(token, "protocol")) {
			conf->protocol = conf_handler_string(strtok_r(NULL, "", &cptr));
			continue;
		}
                if (!strcmp(token, "backlog")) {
			conf->backlog = conf_handler_int(strtok_r(NULL, "", &cptr));
			continue;
		}
	}

	return conf;
}

int
open_socket(rat_conf *conf)
{
	int r_socket;
	int w_socket;
	struct sockaddr_in addr;
	struct sockaddr_in client;
	int len;

	r_socket = socket(AF_INET, SOCK_STREAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(conf->port);
	addr.sin_addr.s_addr = INADDR_ANY;
	bind(r_socket, (struct sockaddr *)&addr, sizeof(addr));
	listen(r_socket, conf->backlog);

	len = sizeof(client);
	w_socket = accept(r_socket, (struct sockaddr *)&client, &len);
	write(w_socket, "HELLO", 5);

	close(w_socket);
	close(r_socket);

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

	open_socket(conf);
	
	printf("%s\n", conf->host);
	printf("%d\n", conf->port);
	printf("%s\n", conf->protocol);
	printf("%d\n", conf->backlog);

	return 0;
}
