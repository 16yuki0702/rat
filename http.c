#include "http.h"

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

http_request
http_request_parse(char *request_line)
{
	http_request *request;
	request = calloc(1, sizeof(http_request));

	strcmp_substr(request_line, "GET", 0, 3);
}
