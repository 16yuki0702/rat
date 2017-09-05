#ifndef RAT_HTTP_H
#define RAT_HTTP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rat_string.h"
#include "rat_log.h"

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

typedef struct {
	char *p;
	int len;
} http_entry;

#define MAKE_HTTP_ENTRY(entry, start, len)	\
	entry.p = start;			\
	entry.len = len;

typedef struct {
	http_entry method;
	http_entry uri;
	http_entry version;
	http_entry host;
	http_entry connection;
	int upgrade_insecure_requests;
	http_entry user_agent;
	http_entry accept;
	http_entry accept_encoding;
	http_entry accept_language;
} http_request2;

extern http_request *rat_request;

extern void http_request_parse(char *request_line);
extern http_request2 *http_request_parse2(char *request);

#endif
