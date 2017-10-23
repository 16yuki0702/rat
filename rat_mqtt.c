#include "rat_mqtt.h"

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

void
send_connack(uint32_t sock, r_mqtt_packet *p)
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
scan_data16(r_str *p, uint8_t *c)
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
handle_subscribe(uint32_t sock, r_mqtt_packet *p, rat_conf *conf)
{
	uint8_t qs[512];
	uint8_t n = 0, *res;
	uint32_t m_len, r_size, pos, len;
	uint8_t qos = 1;
	uint16_t m;
	subscription sub;

	pos = 0;
	sub.sock = sock;

	while (p->payload.l > pos) {
		sub.topic.l = p->payload.d[pos] << 8 | p->payload.d[pos + 1];
		sub.topic.d = &p->payload.d[pos + 2];
		pos += 2 + sub.topic.l;
		qs[n++] = p->payload.d[pos];
		sub.qos = p->payload.d[pos];
		pos += 1;
		store_data(&sub, conf);
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

static void
handle_publish(uint32_t sock, r_mqtt_packet *p, rat_conf *conf)
{
	uint8_t *res;
	uint16_t t_len, m_len;
	uint32_t data_len, len;

	data_len = (2 + p->topic.l + ((p->qos >= 1) ? 2 : 0) + p->payload.l);

	res = (uint8_t*)malloc(sizeof(uint8_t) * data_len);

	t_len = htons(p->topic.l);
	memcpy(res, &t_len, 2);
	memcpy(res + 2, p->topic.d, p->topic.l);

	if (p->qos >= 1) {
		m_len = htons(p->message_id);
		memcpy(res + 2 + p->topic.l, &m_len, 2);
		memcpy(res + 2 + p->topic.l + 2, p->payload.d, p->payload.l);
	} else {
		memcpy(res + 2 + p->topic.l, p->payload.d, p->payload.l);
	}

	len = add_mqtt_header(res, MQTT_PUBLISH, p->qos, data_len);

	publish_data(&p->topic, res, len, conf);

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
			ph = scan_data16(&p->protocol_name, ph);
			p->protocol_version = *ph++;
			p->connect_flags = *ph++;
			p->keepalive_timer = get_uint16(ph);
			ph += 2;
			ph = scan_data16(&p->client_id, ph);
			if (p->connect_flags & MQTT_IS_WILL) {
				ph = scan_data16(&p->will_topic, ph);
				ph = scan_data16(&p->will_message, ph);
			}
			if (p->connect_flags & MQTT_IS_USERNAME) {
				ph = scan_data16(&p->username, ph);
			}
			if (p->connect_flags & MQTT_IS_PASSWORD) {
				ph = scan_data16(&p->password, ph);
			}
			send_connack(sock, p);
			break;
		case MQTT_SUBSCRIBE:
			p->message_id = get_uint16(ph);
			ph += 2;
			p->payload.d = ph;
			p->payload.l = (size_t)(end - ph);
			handle_subscribe(sock, p, c->conf);
			break;
		case MQTT_PUBLISH:
			p->topic.l = ph[0] << 8 | ph[1];
			p->topic.d = &ph[2];
			ph += 2 + p->topic.l;
			if (p->qos >= 1) {
				p->message_id = get_uint16(ph);
				ph += 2;
			}
			p->payload.d = ph;
			p->payload.l = (size_t)(end - ph);
			handle_publish(sock, p, c->conf);
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
