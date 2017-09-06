#ifndef RAT_LOG_H
#define RAT_LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/stat.h>

#define LOG_DIR "log"
#define LOG_FILE "log/rat.log"
#define DEFAULT_CONF_PATH "rat.conf"

extern FILE *rat_log_file;

typedef enum {
	DEBUG,
	INFO,
	WARNING,
	ERROR,
	FATAL
} RAT_LOG_LEVEL;

extern int rat_log_level;

#define LOG(level, str)				\
	do {					\
		if (rat_log_level >= level) { 	\
			log_prefix(__func__);	\
			rat_log str;		\
		}				\
	} while (0)

#define LOG_DEBUG(str)		LOG(DEBUG,	str)
#define LOG_INFO(str)		LOG(INFO,	str)
#define LOG_WARNING(str)	LOG(WARNING,	str)
#define LOG_ERROR(str)		LOG(ERROR,	str)
#define LOG_FATAL(str)		LOG(FATAL,	str)

#define GET_LOG_LEVEL(x)				\
	((x == DEBUG)	? "[DEBUG]"	:		\
	(x == INFO)	? "[INFO]"	:		\
	(x == WARNING)	? "[WARNING]"	:		\
	(x == ERROR)	? "[ERROR]"	:		\
	(x == FATAL)	? "[FATAl]"	: "[UNKNOWN]")

extern void log_prefix(const char *func);

extern void rat_log(const char *fmt, ...);

extern void set_log(FILE *f);

extern FILE *open_log_file(char *filepath);

extern void close_log_file(char *filepath);

#endif
