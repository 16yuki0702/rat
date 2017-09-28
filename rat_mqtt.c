#include "rat_mqtt.h"

char test1[] = "foo";
char test2[] = "foo/bar";
char test3[] = "foo/bar/baz";

typedef struct _map_entry {
	_map_entry *root;
	_map_entry *left;
	_map_entry *right;
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
_parse_topic(char *e_data, char *t)
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
	map.roots = (map_entry**)realloc(sizeof(map_entry*) * resize);
}

void
_test_driver()
{
	_realloc_map(1);
	_parse_topic(test1);
	_realloc_map(2);
	_parse_topic(test2);
	_realloc_map(3);
	_parse_topic(test3);
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

void
parse_mqtt(int sock)
{
	uint8_t byte, type;

	if (read(sock, &byte, sizeof(byte)) == -1) {
		LOG_ERROR(("failed read socket."));
	}

	type = (byte & 0xF0);

	_dec2bin(type);

	if (type == MQTT_CONNECT) {
		printf("connection start\n");

		// test connack packet.
		char test[4] = {0};
		test[0] = 32;
		test[1] = 2;
		test[2] = 0;
		test[3] = 0;
		write(sock, test, 4);
	}
}
