#include "http.h"

http_request *rat_request;

static void _dump_request(void);
static void _dump_http_entry(http_entry e);

static void
_dump_request(void)
{
	_dump_http_entry(rat_request->method);
	_dump_http_entry(rat_request->uri);
	_dump_http_entry(rat_request->version);
	_dump_http_entry(rat_request->host);
	_dump_http_entry(rat_request->connection);
	_dump_http_entry(rat_request->user_agent);
	_dump_http_entry(rat_request->cookie);
}

static void
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
is_entry_same(char *c1, char *c2, int len)
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
http_request_parse(char *request)
{
	int eof = 0, section = 0, len = 0, k_len = 0, v_len = 0;
	char *start, *end, *k_start, *v_start;

	rat_request = (http_request*)malloc(sizeof(http_request));
	memset(rat_request, 0, sizeof(http_request));

	start = end = request;

	// parse request header line
	while (*end) {
		CHECK_EOF();
		if (*end == ' ') {
			len = (end - start);
			switch (section) {
				case HTTP_METHOD:
					MAKE_HTTP_ENTRY(rat_request->method, start, len);
					break;
				case REQUEST_URI:
					MAKE_HTTP_ENTRY(rat_request->uri, start, len);
					break;
				default:
					break;
			}

			start = (end + 1);
			section++;
		} else if (eof) {
			len = (end - start);
			MAKE_HTTP_ENTRY(rat_request->version, start, len);
			end += 2;
			start = end;
			eof = 0;
			break;
		}

		++end;
	}

	// parse request header
	while (*end) {
		CHECK_EOF();
		if (eof) {
			v_start = start;
			v_len = (end - start);
			CHECK_ENTRY(rat_request, k_start, k_len, v_start, v_len);
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

	_dump_request();

	return end;
}
