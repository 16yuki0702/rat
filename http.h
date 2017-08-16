#ifndef RAT_HTTP_H
#define RAT_HTTP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rat_string.h"

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
	char *connection;
	int upgrade_insecure_requests;
	char *user_agent;
	char *accept;
	char *accept_encoding;
	char *accept_language;
} http_request;

extern http_request *rat_request;

extern void http_request_parse(char *request_line);

#endif
