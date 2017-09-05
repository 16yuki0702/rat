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

typedef struct {
	rat_str *method;
	rat_str *uri;
	rat_str *version;
	rat_str *host;
	rat_str *connection;
	int upgrade_insecure_requests;
	rat_str *user_agent;
	rat_str *accept;
	rat_str *accept_encoding;
	rat_str *accept_language;
} http_request;

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
	if (_same(k_start, "Host", k_len)) {				\
		MAKE_HTTP_ENTRY(entry->host, v_start, v_len);		\
	} else if (_same(k_start, "Connection", k_len)) {		\
		MAKE_HTTP_ENTRY(entry->connection, v_start, v_len);	\
	} else if (_same(k_start, "User-Agent", k_len)) {		\
		MAKE_HTTP_ENTRY(entry->user_agent, v_start, v_len);	\
	} else if (_same(k_start, "Cookie", k_len)) {			\
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
} http_request2;

extern http_request *rat_request;

extern void http_request_parse(char *request_line);
extern char *http_request_parse2(char *request);

#endif
