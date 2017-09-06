#ifndef RAT_HTTP_H
#define RAT_HTTP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rat_string.h"
#include "rat_log.h"

#ifndef TRUE
#	define TRUE 1
#endif
#ifndef FALSE
#	define FALSE 0
#endif

#define HTTP_200_RES "" 				\
	"HTTP/1.1 200 OK \r\n" 				\
	"\r\n"

#define HTTP_404_RES "" 				\
	"HTTP/1.1 404 Not Found \r\n" 			\
	"\r\n"

#define HTTP_500_RES "" 				\
	"HTTP/1.1 500 Internal Server Error \r\n"	\
	"\r\n"

#define MAKE_HTTP_ENTRY(e, s, l)					\
	e.p = s;							\
	e.len = l;

#define CHECK_EOF()							\
	if (end[0] == '\r' && end[1] == '\n') {				\
		eof = 1;						\
	}

#define IS_END()							\
	if (end[0] == '\r' && end[1] == '\n') {				\
		break;							\
	}

#define CHECK_ENTRY(entry, k_start, k_len, v_start, v_len)		\
	if (is_entry_same(k_start, "Host", k_len)) {				\
		MAKE_HTTP_ENTRY(entry->host, v_start, v_len);		\
	} else if (is_entry_same(k_start, "Connection", k_len)) {		\
		MAKE_HTTP_ENTRY(entry->connection, v_start, v_len);	\
	} else if (is_entry_same(k_start, "User-Agent", k_len)) {		\
		MAKE_HTTP_ENTRY(entry->user_agent, v_start, v_len);	\
	} else if (is_entry_same(k_start, "Cookie", k_len)) {			\
		MAKE_HTTP_ENTRY(entry->cookie, v_start, v_len);		\
	}

typedef struct {
	char *p;
	int len;
} http_entry;

typedef enum {
	HTTP_METHOD,
	REQUEST_URI
} REQUEST_HEADER_LINE;

typedef struct {
	http_entry method;
	http_entry uri;
	http_entry version;
	http_entry host;
	http_entry connection;
	int upgrade_insecure_requests;
	http_entry user_agent;
	http_entry cookie;
	http_entry accept;
	http_entry accept_encoding;
	http_entry accept_language;
} http_request;

extern http_request *rat_request;

extern char *http_request_parse(char *request);

extern int is_entry_same(char *c1, char *c2, int len);

#endif
