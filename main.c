#include <stdio.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <time.h>
#include "server.h"
#include "config.h"

#define LOG_DIR "log"
#define LOG_FILE "log/rat.log"
#define DEFAULT_CONF_PATH "rat.conf"

FILE *rat_log_file;

#define LOG(level, str)				\
	do {					\
		if (rat_log_level >= level) { 	\
			_log_prefix(__func__);	\
			_log str;		\
		}				\
	} while (0)

void _log_prefix(const char *func)
{
	char prefix[21];
	strncpy(prefix, func, 20);
	prefix[20] = '\0';
	if (rat_log_file == NULL) {
		rat_log_file = stderr;
	}
	fprintf(rat_log_file, "%-20s ", prefix);
}

void _log(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(rat_log_file, fmt, ap);
	va_end(ap);
	fputc('\n', rat_log_file);
	fflush(rat_log_file);
}

void _set_log(FILE *f)
{
	rat_log_file = f;
}

FILE *
_open_log_file(char *filepath)
{
	struct stat s;
	if (lstat(LOG_DIR, &s) == -1) {
		printf("not found %s\n", LOG_DIR);
		mkdir(LOG_DIR, 0755);
	}

	FILE *f;
	f = fopen(filepath, "w");
	if (f == NULL) {
		printf("fail open log file.\n");
		exit(1);
	}

	return f;
}

int rat_log_level = 0;

rat_conf *conf;

typedef enum {
	DEBUG,
	INFO,
	WARNING,
	ERROR,
	CRITICAL
} RAT_LOG_LEVEL;

void
signal_handler(int signal)
{
	printf("signal num = %d\n", signal);
}

void
sighup_handler(int signal)
{
	read_config(DEFAULT_CONF_PATH);
	printf("signal num = %d\n", signal);
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

void
_initialize_time()
{
	time_t timer;
	struct tm *t_st;

	time(&timer);
}

int
main(int argc, char *argv[])
{
	char *conf_path;
	int error_code = 0;

	_initialize_time();

	_set_log(_open_log_file(LOG_FILE));

	LOG(DEBUG, ("test"));

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
