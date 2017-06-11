#include "config.h"

rat_conf *conf;
static int conf_error = 0;

static void
conf_handler_string(char **conf, const char *param)
{
	char *ret;
	int len;

	ret = strdup(param);
	len = strlen(ret);

	if (ret[len - 1] == '\n') {
		ret[len - 1] = '\0';
	}

	*conf = ret;
}

static void
conf_handler_int(int *conf, const char *param)
{
	int ret;

	ret = atoi(param);

	*conf = ret;
}

static void
conf_handler_uint16(uint16_t *conf, const char *param)
{
	uint16_t ret;

	ret = atoi(param);

	*conf = ret;
}

static rat_conf *
_read_config(char *path)
{
	FILE *f;
	char *str;
	char buf[1024];
	char *token;
	char *cptr;

	conf = (rat_conf *)malloc(sizeof(rat_conf));
	memset(conf, 0, sizeof(conf));

	f = fopen(path, "rt");
	while (fgets(buf, 1024, f)) {
		token = strtok_r(buf, " ", &cptr);
		if (!strcmp(token, "#")) {
			// skip comment section.
		} else if (!strcmp(token, "host")) {
			conf_handler_string(&conf->host, strtok_r(NULL, "", &cptr));
		} else if (!strcmp(token, "port")) {
			conf_handler_uint16(&conf->port, strtok_r(NULL, "", &cptr));
		} else if (!strcmp(token, "protocol")) {
			conf_handler_string(&conf->protocol, strtok_r(NULL, "", &cptr));
		} else if (!strcmp(token, "backlog")) {
			conf_handler_uint16(&conf->backlog, strtok_r(NULL, "", &cptr));
		} else if (!strcmp(token, "socket_reuse")) {
			conf_handler_int(&conf->socket_reuse, strtok_r(NULL, "", &cptr));
		} else if (!strcmp(token, "use_epoll")) {
			conf_handler_int(&conf->use_epoll, strtok_r(NULL, "", &cptr));
		} else if (!strcmp(token, "non_blocking")) {
			conf_handler_int(&conf->use_epoll, strtok_r(NULL, "", &cptr));
		}
	}
}

int
read_config(char *path)
{
	_read_config(path);

	if (conf_error) {
		printf("config error.\n");
		return -1;
	}

	return 0;
}
