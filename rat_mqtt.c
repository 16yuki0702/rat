#include "rat_mqtt.h"

#define BUF_SIZE			1024

char test1[] = "foo";
char test2[] = "foo/bar";
char test3[] = "foo/bar/baz";

typedef struct _map_entry {
	struct _map_entry *root;
	struct _map_entry *left;
	struct _map_entry *right;
	char *data;
} map_entry;

typedef struct {
	map_entry **roots;
	int count;
} mapper;

mapper map;

typedef enum {
	SAME = 0,
	NOT_SAME,
	LEFT,
	RIGHT
} CMP_RES;

typedef struct {
	uint8_t h1;
	uint8_t h2;
	uint8_t *b;
	uint8_t *serialize_data;
	int serialize_data_len;
} mqtt_packet;

typedef struct {
	int8_t *d;
	int len;
} buf;

CMP_RES
_strcmpn(char *s1, char *s2, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		if (!s1[i] && !s2[i]) {
			return SAME;
		} else if (!s1[i] && s2[i]) {
			return RIGHT;
		} else if (s1[i] && !s2[i]) {
			return LEFT;
		} else if (s1[i] != s2[i]) {
			return NOT_SAME;
		}
	}

	return SAME;
}

void
_parse_topic(char *data, char *t)
{
	char *start;
	int c = 0;
	CMP_RES r;

	while (t[c]) {
		start = t;
		if (t[c] == '/') {
			r = _strcmpn(data, t, strlen(t));
		}
		c++;
	}
}

void
_realloc_map(int resize)
{
	map.roots = (map_entry**)realloc(map.roots, sizeof(map_entry*) * resize);
	if (map.roots == NULL) {
		LOG_FATAL(("exhausted memory."));
	}
}

void
__free_map(map_entry *e)
{
	if (e->left) {
		__free_map(e->left);
	}
	if (e->right) {
		__free_map(e->right);
	}
	free(e->data);
	free(e);
}

void
_free_map(int size)
{
	int i;
	for (i = 0; i < size; i++) {
		map_entry *e;
		e = map.roots[i];
		__free_map(e);
	}
	free(map.roots);
}

void
_test_driver()
{
	_realloc_map(1);

	map_entry *e = map.roots[0];

	_realloc_map(1);
	_parse_topic(e->data, test1);
	_realloc_map(2);
	_parse_topic(e->data, test2);
	_realloc_map(3);
	_parse_topic(e->data, test3);
}

void
_dec2bin(int dec)
{
	int bin = 0, base = 1;

	while (dec > 0) {
		bin += (dec % 2) * base;
		dec /= 2;
		base *= 10;
	}

	printf("%d\n", bin);
}

mqtt_packet *
_create_mqtt_packet(uint8_t *data, int size, uint8_t state)
{
	mqtt_packet *p;
	p = (mqtt_packet*)malloc(sizeof(mqtt_packet));
	memset(p, 0, sizeof(mqtt_packet));

	p->h1 |= state;

	if (size == 0 || data == NULL) {
		p->h2 = 2;
		p->b = (uint8_t*)malloc(sizeof(uint8_t) * 2);
		p->b[0] = 0;
		p->b[1] = 0;
	} else {
		p->h2 = (size / sizeof(uint8_t));
		strncpy(p->b, data, size);
	}

	p->serialize_data = NULL;
	p->serialize_data_len = 0;

	return p;
}

void
_serialize_mqtt_packet(mqtt_packet *p)
{
	int i, serial_size;
	uint8_t *r;

	serial_size = (sizeof(uint8_t) * p->h2) + (sizeof(uint8_t) * MQTT_HEADER_SIZE);
	p->serialize_data = (uint8_t*)malloc(serial_size);
	memset(p->serialize_data, 0, serial_size);

	r = p->serialize_data;

	r[0] |= p->h1;
	r[1] |= p->h2;

	for (i = 0; i < p->h2; i++) {
		r[i+2] |= p->b[i];
	}

	p->serialize_data_len = serial_size;
}

buf *
_read_socket(int sock)
{
	int8_t tmp[BUF_SIZE];
	int n;
	buf *r;

	r = (buf*)malloc(sizeof(buf));
	memset(r, 0, sizeof(buf));

	if ((n = read(sock, tmp, BUF_SIZE)) > 0) {
		r->len += n;
		r->d = (int8_t*)realloc(r->d, r->len);
		memcpy(&r->d[r->len - n], tmp, n);
	} else if (n == -1) {
		LOG_ERROR(("failed read socket."));
	}

	return r;
}

#define MQTT_CONNACK_REMAINLEN	2

#define MQTT_GET_CMD(f)		(((f) & 0xF0) >> 4)
#define MQTT_GET_DUP(f)		(((f) & 0x08) >> 3)
#define MQTT_GET_QOS(f)		(((f) & 0x06) >> 1)
#define MQTT_GET_RETAIN(f)	((f)  & 0x01)
//#define MQTT_CALC_REMAIN(r)			\

typedef enum {
	CONNACK_ALLOW,
	CONNACK_DENY_PROTOCOL,
	CONNACK_DENY_ID,
	CONNACK_DENY_SERVER_ERROR,
	CONNACK_DENY_AUTH_FAILURE,
	CONNACK_DENY_NOT_PERMITTED
} MQTT_CONNACK_RC;

void
_send_connack(int sock)
{
	char p[4] = {0};
	p[0] = MQTT_CONNACK;
	p[1] = MQTT_CONNACK_REMAINLEN;
	p[2] = 0;
	p[3] = CONNACK_ALLOW;

	write(sock, p, 4);
}

typedef struct {
	uint8_t cmd:4;
	uint8_t dup:1;
	uint8_t qos:2;
	uint8_t retain:1;
	uint32_t remain;
	r_str protocol_name;
	uint8_t protocol_version;
	uint8_t connect_flags;
	uint16_t keepalive_timer;
} r_mqtt_packet;

static uint16_t
get_uint16(char *p)
{
	uint8_t *tmp = (uint8_t *)p;
	return (tmp[0] << 8) + tmp[1];
}

static r_mqtt_packet *
init_mqtt_packet()
{
	r_mqtt_packet *p;
	p = (r_mqtt_packet*)malloc(sizeof(r_mqtt_packet));
	memset(p, 0, sizeof(r_mqtt_packet));
	return p;
}

static uint8_t *
scan_data(r_str *p, uint8_t *c, uint8_t size)
{
	p->l = get_uint16(c);
	p->d = c + size;
	return p->d + p->l;
}

void
parse_mqtt(int sock)
{
	r_mqtt_packet *p;
	uint8_t cmd, h1, *ph;
	int remain = 0, mul = 1;
	buf *b;

	b = _read_socket(sock);
	h1 = b->d[0];
	ph = &b->d[1];

	p = init_mqtt_packet();
	p->cmd = MQTT_GET_CMD(h1);
	p->dup = MQTT_GET_DUP(h1);
	p->qos = MQTT_GET_QOS(h1);
	p->retain = MQTT_GET_RETAIN(h1);

	do {
		remain += (*ph & 0x7F) * mul;
		mul <<= 7;
	} while ((*ph++ & 0x80) != 0);

	p->remain = remain;

	switch (cmd) {
		case MQTT_CONNECT:
			ph = scan_data(&p->protocol_name, ph, 2);

			_send_connack(sock);
			break;
		default:
			break;
	}
}
