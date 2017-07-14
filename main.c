#include <signal.h>
#include "server.h"
#include "config.h"

#define LOG(level, str)				\
	do {					\
		if (rat_log_level >= level) { 	\
			_log x;			\
		}				\
	} while (0)

int rat_log_level = 0;

rat_conf *conf;

void
signal_handler(int signal)
{
	printf("signal num = %d\n", signal);
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
		return -1;
	}

	if (signal(SIGINT, signal_handler) == SIG_ERR) {
		printf("couldn't set signal.\n");
		return -1;
	}

	open_socket(conf);
	
	return 0;
}
