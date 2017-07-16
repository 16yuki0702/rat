#include <stdio.h>
#include <signal.h>
#include "server.h"
#include "config.h"

FILE *rat_log_file;

#define LOG(level, str)				\
	do {					\
		if (rat_log_level >= level) { 	\
			_log_prefix(__func__);	\
			_log x;			\
		}				\
	} while (0)

void _log_prefix(const char *func) {
	char prefix[21];
	strncpy(prefix, func, 20);
	prefix[20] = '\0';
	if (rat_log_file == NULL) {
		rat_log_file = stderr;
	}
	fprintf(rat_log_file, "%-20s ", prefix);
}

void _log(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(rat_log_file, fmt, ap);
	va_end(ap);
	fputc('\n', rat_log_file);
	fflush(rat_log_file);
}

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
