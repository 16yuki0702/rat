#ifndef RAT_LOG_H
#define RAT_LOG_H

#define LOG_DIR "log"
#define LOG_FILE "log/rat.log"
#define DEFAULT_CONF_PATH "rat.conf"

FILE *rat_log_file;

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

void log_prefix(const char *func);

void log(const char *fmt, ...);

void set_log(FILE *f);

FILE *open_log_file(char *filepath);

#endif
