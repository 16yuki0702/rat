#ifndef RAT_MQTT_H
#define RAT_MQTT_H

#define BUF_SIZE			1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "rat.h"
#include "rat_log.h"
#include "rat_string.h"

#define MQTT_CONNECT		0x01
#define MQTT_CONNACK		0x02
#define MQTT_PUBLISH		0x03
#define MQTT_PUBACK		0x04
#define MQTT_PUBREC		0x05
#define MQTT_PUBREL		0x06
#define MQTT_PUBCOMP		0x07
#define MQTT_SUBSCRIBE		0x08
#define MQTT_SUBACK		0x09
#define MQTT_UNSUBSCRIBE	0x0A
#define MQTT_UNSUBACK		0x0B
#define MQTT_PINGREQ		0x0C
#define MQTT_PINGRESP		0x0D
#define MQTT_DISCONNECT		0x0E

#define MQTT_HEADER_SIZE	2
#define MQTT_CONNACK_REMAINLEN	2

#define MQTT_GET_CMD(f)		(((f) & 0xF0) >> 4)
#define MQTT_GET_DUP(f)		(((f) & 0x08) >> 3)
#define MQTT_GET_QOS(f)		(((f) & 0x06) >> 1)
#define MQTT_GET_RETAIN(f)	((f)  & 0x01)
#define MQTT_IS_WILL		0x04
#define MQTT_IS_USERNAME	0x40
#define MQTT_IS_PASSWORD	0x80

extern void parse_mqtt(int sock);

typedef enum {
	CONNECTING
} r_mqtt_state;

typedef struct {
	int sock;
	r_mqtt_state state;
} r_mqtt_conn;

#endif
