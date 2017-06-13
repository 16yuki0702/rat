#include "http.h"

http_request *rat_request;

static char*
substr(char const *str, int start, int end)
{
	char *dst, *ret;
	int i, strlen = end - start;
	if (strlen < 0) {
		printf("strlen minus error.\n");
		return NULL;
	}
	dst = calloc(1, strlen);	
	ret = dst;
	for (i = start; i < end; i++) {
		*dst++ = str[i];
	}

	return ret;
}

static int
strcmp_substr(char const *src, char const *dst, int start, int end)
{
	int ret = 0;
	char *check = substr(src, 0, 3);

	if (ret = !strcmp(check, dst)) {
		printf("same!!\n");
	} else {
		printf("not same.\n");	
	}

	free(check);	

	return ret;
}

static void
_parse_header_line(char *request_line)
{
	char *token;
	char *cptr;

	rat_request->method = strtok_r(request_line, " ", &cptr);
	rat_request->uri = strtok_r(NULL, " ", &cptr);
	strtok_r(NULL, "/", &cptr);
	rat_request->version = strtok_r(NULL, "\n", &cptr);
}

void
http_request_parse(char *request_line)
{
	char *cp;
	cp = strdup(request_line);

	rat_request = calloc(1, sizeof(http_request));

	_parse_header_line(cp);

	printf("%s\n", rat_request->method);
	printf("%s\n", rat_request->uri);
	printf("%s\n", rat_request->version);

	//printf("request %s\n", request_line);
}
