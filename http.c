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
	if (rat_request->method)			_DEBUG(("method:%s\n", rat_request->method->data));
	if (rat_request->uri)				_DEBUG(("uri:%s\n", rat_request->uri->data));
	if (rat_request->version)			_DEBUG(("version:%s\n", rat_request->version->data));
	if (rat_request->host)				_DEBUG(("host:%s\n", rat_request->host->data));
	if (rat_request->connection)			_DEBUG(("connection:%s\n", rat_request->connection->data));
	if (rat_request->upgrade_insecure_requests)	_DEBUG(("upgrade_insecure_requests:%d\n", rat_request->upgrade_insecure_requests));
	if (rat_request->user_agent)			_DEBUG(("user_agent:%s\n", rat_request->user_agent->data));
	if (rat_request->accept)			_DEBUG(("accept:%s\n", rat_request->accept->data));
	if (rat_request->accept_encoding)		_DEBUG(("accept_encoding:%s\n", rat_request->accept_encoding->data));
	if (rat_request->accept_language)		_DEBUG(("accept_language:%s\n", rat_request->accept_language->data));
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
		rat_request->user_agent = make_rat_str(value);
	} else if (!strcmp(key, "Accept")) {
		rat_request->accept = make_rat_str(value);
	} else if (!strcmp(key, "Accept-Encoding")) {
		rat_request->accept_encoding = make_rat_str(value);
	} else if (!strcmp(key, "Accept-Language")) {
		rat_request->accept_language = make_rat_str(value);
	}
}

static char*
substr(char const *str, int start, int end)
{
	char *dst, *ret;
	int i, strlen = end - start;
	if (strlen < 0) {
		_ERROR(("strlen minus error.\n"));
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
		_DEBUG(("same!!\n"));
	} else {
		_DEBUG(("not same.\n"));
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

	_dump_request();
}

#define CHECK_EOF()					\
	if (c_pos[0] == '\r' && c_pos[1] == '\n') {	\
		eof = 1;				\
	}

typedef enum {
	HTTP_METHOD,
	REQUEST_URI,
	HTTP_VERSION
} REQUEST_HEADER_LINE;

http_request *
http_request_parse2(char *request_line)
{
	int eof = 0, hit = 0;
	size_t diff = 0;
	char *ctrl_p, *c_pos;
	char method[8] = {0}, uri[256] = {0}, version[16] = {0};
	http_request *r;

	r = (http_request*)malloc(sizeof(http_request));
	memset(r, 0, sizeof(http_request));

	ctrl_p = c_pos = request_line;

	// parse request header line
	while (*c_pos) {
		CHECK_EOF();
		if (*c_pos == ' ') {
			diff = (c_pos - ctrl_p);
			switch (hit) {
				case HTTP_METHOD:
					r->method = make_rat_strn(ctrl_p, diff);
					break;
				case REQUEST_URI:
					r->uri = make_rat_strn(ctrl_p, diff);
					break;
<<<<<<< HEAD
				case HTTP_VERSION:
					r->version = make_rat_strn(ctrl_p, diff);
					break;
=======
>>>>>>> ce0d88d20cbf2de1c5e8b2048324f21270e54dcd
				default:
					break;
			}

			ctrl_p = (c_pos + 1);
			hit++;
		} else if (eof) {
			diff = (c_pos - ctrl_p);
			strncpy(version, ctrl_p, diff);
			c_pos += 2;
			ctrl_p = c_pos;
			eof = 0;
			goto REQUEST_LINE_PARSE_END;
		}

		++c_pos;
	}

REQUEST_LINE_PARSE_END:
	printf("request = %s\n", request_line);
	printf("method = %s\n", method);
	printf("uri = %s\n", uri);
	printf("version = %s\n", version);

	char k[64] = {0}, v[64] = {0};

	while (*c_pos) {
		CHECK_EOF();
		if (eof) {
			diff = (c_pos - ctrl_p);
			memset(v, 0, 64);
			strncpy(v, ctrl_p, diff);
			c_pos += 2;
			ctrl_p = c_pos;
			printf("k = %s\n", k);
			printf("v = %s\n", v);
			eof = 0;
			continue;
		}

		if (*c_pos == ':') {
			diff = (c_pos - ctrl_p);
			memset(k, 0, 64);
			strncpy(k, ctrl_p, diff);
			c_pos += 2;
			ctrl_p = c_pos;
			continue;
		}

		++c_pos;
	}

	return r;
}
