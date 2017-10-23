#include "rat_mqtt.h"

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
	uint8_t *d;
	int len;
} buf;

void
free_buf(buf *b)
{
	free(b->d);
	free(b);
}

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
		r->d = (uint8_t*)realloc(r->d, r->len);
		memcpy(&r->d[r->len - n], tmp, n);
	} else if (n == -1) {
		LOG_ERROR(("failed read socket."));
	}

	return r;
}

typedef enum {
	CONNACK_ALLOW,
	CONNACK_DENY_PROTOCOL,
	CONNACK_DENY_ID,
	CONNACK_DENY_SERVER_ERROR,
	CONNACK_DENY_AUTH_FAILURE,
	CONNACK_DENY_NOT_PERMITTED
} MQTT_CONNACK_RC;

void
_send_connack(uint32_t sock, r_mqtt_packet *p)
{
	char rp[4] = {0};
	rp[0] = MQTT_CONNACK << 4;
	rp[1] = MQTT_CONNACK_REMAINLEN;
	rp[2] = 0;
	rp[3] = CONNACK_ALLOW;

	write(sock, rp, 4);
	//(void*)p;
}

static uint16_t
get_uint16(uint8_t *p)
{
	uint8_t *tmp = p;
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
scan_data(r_str *p, uint8_t *c)
{
	p->l = get_uint16(c);
	p->d = c + 2;
	return p->d + p->l;
}

uint8_t
add_mqtt_header(uint8_t *dst, uint8_t cmd, uint8_t qos, uint32_t len)
{
	uint8_t header[5];
	uint8_t i = 1, remain, resize;
	uint8_t *tmp;

	header[0] = (cmd << 4) | (qos << 1);
	remain = len;

	do {
		header[i] = remain % 0x80;
		remain /= 0x80;
		if (remain > 0) {
			header[i] |= 0x80;
		}
		i++;
	} while (remain > 0);

	resize = i + len;

	tmp = (uint8_t*)realloc(dst, resize);
	if (tmp == NULL) {
		printf("memory allocate error.\n");
		exit(EXIT_FAILURE);
	}

	dst = tmp;

	memmove(dst + i, dst, len);
	memcpy(dst, header, i);

	return resize;
}

static void
handle_subscribe(uint32_t sock, r_mqtt_packet *p)
{
	uint8_t qs[512];
	uint8_t n = 0, *res;
	uint32_t t_len, m_len, r_size, pos, len;
	uint8_t qos = 1;
	uint16_t m;

	// TODO store the topic data relating each conneciton to.
/*
	p->topic.l = p->payload.d[0] << 8 | p->payload.d[1];
	p->topic.d = p->payload.d + 2;
	p->qos = p->payload.d[2 + p->topic.l];
*/
	pos = 0;

	while (p->payload.l > pos) {
		t_len = p->payload.d[pos] << 8 | p->payload.d[pos + 1];
		pos += 2 + t_len;
		qs[n++] = p->payload.d[pos++];
	}

	m_len = sizeof(p->message_id);
	m = htons(p->message_id);
	r_size = m_len + n;

	res = (uint8_t*)malloc(sizeof(uint8_t) * (r_size));
	memcpy(res, &m, m_len);
	memcpy(res + m_len, qs, n);
	len = add_mqtt_header(res, MQTT_SUBACK, qos, r_size);

	write(sock, res, len);

	free(res);
}

void
parse_mqtt(r_connection *c)
{
	r_mqtt_packet *p;
	uint8_t h1, *ph, *end;
	int remain = 0, mul = 1, sock;
	buf *b;

	sock = c->sock;

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
	end = ph + remain;

	switch (p->cmd) {
		case MQTT_CONNECT:
			ph = scan_data(&p->protocol_name, ph);
			p->protocol_version = *ph++;
			p->connect_flags = *ph++;
			p->keepalive_timer = get_uint16(ph);
			ph += 2;
			ph = scan_data(&p->client_id, ph);
			if (p->connect_flags & MQTT_IS_WILL) {
				ph = scan_data(&p->will_topic, ph);
				ph = scan_data(&p->will_message, ph);
			}
			if (p->connect_flags & MQTT_IS_USERNAME) {
				ph = scan_data(&p->username, ph);
			}
			if (p->connect_flags & MQTT_IS_PASSWORD) {
				ph = scan_data(&p->password, ph);
			}
			_send_connack(sock, p);
			break;
		case MQTT_SUBSCRIBE:
			p->message_id = get_uint16(ph);
			ph += 2;
			p->payload.d = ph;
			p->payload.l = (size_t)(end - ph);
			handle_subscribe(sock, p);
			break;
		case MQTT_PUBLISH:
			ph = scan_data(&p->topic, ph);
			p->message_id = get_uint16(ph);
			ph += 2;
			p->payload.d = ph;
			p->payload.l = (size_t)(end - ph);
			break;
		case MQTT_DISCONNECT:
			close(sock);
			break;
		default:
			send(sock, "test", 4, 0);
			break;
	}

	free_buf(b);
}
