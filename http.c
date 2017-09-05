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

void
_dump_http_entry(http_entry e)
{
	int i;
	char *start = e.p;

	for (i = 0; i < e.len; i++) {
		printf("%c", e.p[i]);
	}
	printf("\n");
}

int
_same(char *c1, char *c2, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		if ((!c1[i] || !c2[i]) || c1[i] != c2[i]) {
			return FALSE;
		}
	}
	return TRUE;
}

char *
http_request_parse2(char *request)
{
	int eof = 0, section = 0, len = 0, k_len = 0, v_len = 0;
	char *start, *end, *k_start, *v_start;
	http_request2 *r;

	r = (http_request2*)malloc(sizeof(http_request2));
	memset(r, 0, sizeof(http_request2));

	start = end = request;

	// parse request header line
	while (*end) {
		CHECK_EOF();
		if (*end == ' ') {
			len = (end - start);
			switch (section) {
				case HTTP_METHOD:
					MAKE_HTTP_ENTRY(r->method, start, len);
					break;
				case REQUEST_URI:
					MAKE_HTTP_ENTRY(r->uri, start, len);
					break;
				default:
					break;
			}

			start = (end + 1);
			section++;
		} else if (eof) {
			len = (end - start);
			MAKE_HTTP_ENTRY(r->version, start, len);
			end += 2;
			start = end;
			eof = 0;
			goto REQUEST_LINE_PARSE_END;
		}

		++end;
	}

REQUEST_LINE_PARSE_END:

	while (*end) {
		CHECK_EOF();
		if (eof) {
			v_start = start;
			v_len = (end - start);
			CHECK_ENTRY(r, k_start, k_len, v_start, v_len);
			end += 2;
			start = end;
			eof = 0;
			IS_END();
			continue;
		}

		if (*end == ':') {
			k_start = start;
			k_len = (end - start);
			end += 2;
			start = end;
			continue;
		}

		++end;
	}

	printf("request = %s\n", request);
	_dump_http_entry(r->method);
	_dump_http_entry(r->uri);
	_dump_http_entry(r->version);
	_dump_http_entry(r->host);
	_dump_http_entry(r->connection);
	_dump_http_entry(r->user_agent);
	_dump_http_entry(r->cookie);

	return end;
}
