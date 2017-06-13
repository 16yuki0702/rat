#ifndef RAT_HTTP_H
#define RAT_HTTP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HTTP_200_RES "" 		\
	"HTTP/1.1 200 OK \r\n" 		\
	"\r\n"

typedef struct {
	char *method;
	char *uri;
	char *version;
	char *host;
	char *connection;
	int upgrate_insecure_requests:1;
	char *user_agent;
	char *accept;
	char *accept_encoding;
	char *accept_language;
} http_request;

extern http_request *rat_request;

extern http_request http_request_parse(char *request_line);

#endif
