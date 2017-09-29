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

void
parse_mqtt(int sock)
{
	mqtt_packet *p;
	uint8_t cmd;
	int8_t *rp;
	int d_len = 0;
	buf *b;

	b = _read_socket(sock);

	cmd = (b->d[0] & 0xF0);
	rp = &b->d[1];

	do {
		d_len += ((*rp & 0x7F)) << 7 * (rp - &b->d[1]);
		printf("d_len %d\n", *rp);
	} while ((*rp++ & 0x80) != 0);

	d_len += (*(--rp) & 0x7F);

	printf("d_len %d\n", d_len);
	//_dec2bin(cmd);
	
	if (cmd == MQTT_CONNECT) {
		p = _create_mqtt_packet(NULL, 0, MQTT_CONNACK);
		_serialize_mqtt_packet(p);
		
		// send connack packet.
		write(sock, p->serialize_data, p->serialize_data_len);
	}
}
