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
rat_conf *conf;

typedef enum {
	DEBUG,
	INFO,
	WARNING,
	ERROR,
	FATAL
} RAT_LOG_LEVEL;

int rat_log_level = DEBUG;

#define LOG(level, str)				\
	do {					\
		if (rat_log_level >= level) { 	\
			_log_prefix(__func__);	\
			_log str;		\
		}				\
	} while (0)

#define _DEBUG(str)	LOG(DEBUG,	str)
#define _INFO(str)	LOG(INFO,	str)
#define _WARNING(str)	LOG(WARNING,	str)
#define _ERROR(str)	LOG(ERROR,	str)
#define _FATAL(str)	LOG(FATAL,	str)

#define GET_LOG_LEVEL(x)				\
	((x == DEBUG)	? "[DEBUG]"	:		\
	(x == INFO)	? "[INFO]"	:		\
	(x == WARNING)	? "[WARNING]"	:		\
	(x == ERROR)	? "[ERROR]"	:		\
	(x == FATAL)	? "[FATAl]"	: "[UNKNOWN]")

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
_log_prefix(const char *func)
{
	fprintf(rat_log_file, "%s\t", GET_LOG_LEVEL(rat_log_level));

	time_t t;
	char date[20] = {0};

	time(&t);
	strftime(date, sizeof(date), "%Y/%m/%d %H:%M:%S\t", localtime(&t));

	fprintf(rat_log_file, "%s ", date);

	char prefix[31];
	strncpy(prefix, func, 30);
	prefix[30] = '\0';
	if (rat_log_file == NULL) {
		rat_log_file = stderr;
	}
	fprintf(rat_log_file, "%-30s\t", prefix);
}

void
_log(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(rat_log_file, fmt, ap);
	va_end(ap);
	fputc('\n', rat_log_file);
	fflush(rat_log_file);
}

void
_set_log(FILE *f)
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
	f = fopen(filepath, "a");
	if (f == NULL) {
		printf("fail open log file.\n");
		exit(1);
	}

	return f;
}


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

int
main(int argc, char *argv[])
{
	char *conf_path;
	int error_code = 0;

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
