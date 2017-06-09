#include "server.h"
#include "config.h"

rat_conf *conf;

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
