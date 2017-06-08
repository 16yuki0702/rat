typedef struct {
	uint16_t port;
	char *host;
	char *protocol;
	uint16_t backlog;
	int socket_reuse;
} rat_conf;

extern rat_conf *conf;

extern void conf_handler_string(char **conf, const char *param);
extern void conf_handler_int(int *conf, const char *param);
extern void conf_handler_uint16(uint16_t *conf, const char *param);
extern int *read_config(char *path);
