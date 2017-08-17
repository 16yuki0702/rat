#include "http.h"

http_request *rat_request;

static void
_trim(char *str)
{
	int i;
	if (str == NULL) {
		return;
	}

	i = strlen(str);

	while (--i >= 0 && str[i] == ' ');

	str[i + 1] = '\0';

	i = 0;
	while (str[i] != '\0' && str[i] == ' ') i++;
	strcpy(str, &str[i]);
}

static void
_dump_request(void)
{
	if (rat_request->method) printf("method:%s\n", rat_request->method->data);
	if (rat_request->uri) printf("uri:%s\n", rat_request->uri->data);
	if (rat_request->version) printf("version:%s\n", rat_request->version->data);
	if (rat_request->host) printf("host:%s\n", rat_request->host->data);
	if (rat_request->connection) printf("connection:%s\n", rat_request->connection->data);
	if (rat_request->upgrade_insecure_requests) printf("upgrade_insecure_requests:%d\n", rat_request->upgrade_insecure_requests);
	if (rat_request->user_agent) printf("user_agent:%s\n", rat_request->user_agent);
	if (rat_request->accept) printf("accept:%s\n", rat_request->accept);
	if (rat_request->accept_encoding) printf("accept_encoding:%s\n", rat_request->accept_encoding);
	if (rat_request->accept_language) printf("accept_language:%s\n", rat_request->accept_language);
}

static void
_set_request_parameter(char *key, char *value)
{
	_trim(value);
	if (!strcmp(key, "Host")) {
		rat_request->host = make_rat_str(value);
	} else if (!strcmp(key, "Connection")) {
		rat_request->connection = make_rat_str(value);
	} else if (!strcmp(key, "Upgrade-Insecure-Requests")) {
		rat_request->upgrade_insecure_requests = atoi(value);
	} else if (!strcmp(key, "User-Agent")) {
		rat_request->user_agent = value;
	} else if (!strcmp(key, "Accept")) {
		rat_request->accept = value;
	} else if (!strcmp(key, "Accept-Encoding")) {
		rat_request->accept_encoding = value;
	} else if (!strcmp(key, "Accept-Language")) {
		rat_request->accept_language = value;
	}
}

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

static char*
_parse_header_line(char *request_line)
{
	char *token;
	char *cptr;

	rat_request->method = make_rat_str(strtok_r(request_line, " ", &cptr));
	rat_request->uri = make_rat_str(strtok_r(NULL, " ", &cptr));
	strtok_r(NULL, "/", &cptr);
	rat_request->version = make_rat_str(strtok_r(NULL, "\n", &cptr));

	return cptr;
}

static void
_parse_header_body(char *request_line)
{
	char *key;
	char *value;
	char *cptr;

	key = strtok_r(request_line, ":", &cptr);

	if (!key) {
		return;	
	}

	do {
		value = strtok_r(NULL, "\n", &cptr);
		_set_request_parameter(key, value);
		if (!cptr) {
			break;
		}

		key = strtok_r(NULL, ":", &cptr);
	} while (key);
}

void
http_request_parse(char *request_line)
{
	char *cp;
	char *cptr;

	cp = strdup(request_line);

	rat_request = calloc(1, sizeof(http_request));

	cptr = _parse_header_line(cp);
	_parse_header_body(cptr);

	//_dump_request();
}
