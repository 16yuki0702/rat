#include "rat_log.h"

FILE *rat_log_file;

int rat_log_level = DEBUG;

void
log_prefix(const char *func)
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
rat_log(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(rat_log_file, fmt, ap);
	va_end(ap);
	fputc('\n', rat_log_file);
	fflush(rat_log_file);
}

void
set_log(FILE *f)
{
	rat_log_file = f;
}

FILE *
open_log_file(char *filepath)
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
