#include <stdio.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include "server.h"
#include "rat_config.h"
#include "rat_log.h"

rat_conf *conf;

double
rat_time(void)
{
	double now;
	struct timeval tv;
	if (gettimeofday(&tv, NULL) != 0) {
		return 0;
	}
	now = (double) tv.tv_sec + (((double) tv.tv_usec) / 1000000.0);

	return now;
}

void
signal_handler(int signal)
{
	_DEBUG(("signal num = %d\n", signal));
}

void
sighup_handler(int signal)
{
	read_config(DEFAULT_CONF_PATH);

	close(rat_log_file);

	set_log(open_log_file(LOG_FILE));

	_DEBUG(("signal num = %d\n", signal));
}

void
set_signal()
{
	struct sigaction sa;

	sa.sa_handler = signal_handler;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGINT);
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) < 0) {
		printf("can't catch SIGINT\n");
	}

	sa.sa_handler = sighup_handler;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGINT);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0) {
		printf("can't catch SIGHUP\n");
	}
}

int
main(int argc, char *argv[])
{
	char *conf_path;
	int error_code = 0;

	set_log(open_log_file(LOG_FILE));

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

	set_signal();

	open_socket(conf);
	
	return 0;
}
