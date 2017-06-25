#include "config.h"

rat_conf *conf;
config_detail conf_error;

static void
_dump_config(void)
{
	printf("port : %d\n",		conf->port);
	printf("host : %s\n",		conf->host);
	printf("protocol : %s\n",	conf->protocol);
	printf("backlog : %d\n", 	conf->backlog);
	printf("socket_reuse : %d\n", 	conf->socket_reuse);
	printf("use_epoll : %d\n", 	conf->use_epoll);
}

static void
_conf_handler_string(char **conf, const char *param)
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
_check_number(const char *param)
{
	char *str = (char *)param;;

	do {

		if (*str >= '0' && *str <= '9') {
			continue;
		} else if (*str == '\r' || *str == '\n' || *str == '\0') {
			continue;
		}

		conf_error.r_code = 1;
		conf_error.r_message = "int type config is not number.";
		break;

	} while (*str++);
}

static void
_conf_handler_int(int *conf, const char *param)
{
	int ret;

	_check_number(param);

	ret = atoi(param);

	*conf = ret;
}

static void
_conf_handler_uint16(uint16_t *conf, const char *param)
{
	uint16_t ret;

	_check_number(param);

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
			_conf_handler_string(&conf->host, strtok_r(NULL, "", &cptr));
		} else if (!strcmp(token, "port")) {
			_conf_handler_uint16(&conf->port, strtok_r(NULL, "", &cptr));
		} else if (!strcmp(token, "protocol")) {
			_conf_handler_string(&conf->protocol, strtok_r(NULL, "", &cptr));
		} else if (!strcmp(token, "backlog")) {
			_conf_handler_uint16(&conf->backlog, strtok_r(NULL, "", &cptr));
		} else if (!strcmp(token, "socket_reuse")) {
			_conf_handler_int(&conf->socket_reuse, strtok_r(NULL, "", &cptr));
		} else if (!strcmp(token, "use_epoll")) {
			_conf_handler_int(&conf->use_epoll, strtok_r(NULL, "", &cptr));
		}
	}
}

int
read_config(char *path)
{
	conf_error.r_code = 0;
	conf_error.r_message = "no error.";

	_read_config(path);

	if (conf_error.r_code) {
		printf("config error : %s\n", conf_error.r_message);
		return -1;
	}

	//_dump_config();

	return 0;
}
