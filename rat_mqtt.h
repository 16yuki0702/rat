#ifndef RAT_MQTT_H
#define RAT_MQTT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "rat.h"
#include "rat_log.h"
#include "rat_string.h"

#define MQTT_CONNECT		0x10
#define MQTT_CONNACK		0x20
#define MQTT_PUBLISH		0x30
#define MQTT_PUBACK		0x40
#define MQTT_PUBREC		0x50
#define MQTT_PUBREL		0x60
#define MQTT_PUBCOMP		0x70
#define MQTT_SUBSCRIBE		0x80
#define MQTT_SUBACK		0x90
#define MQTT_UNSUBSCRIBE	0xA0
#define MQTT_UNSUBACK		0xB0
#define MQTT_PINGREQ		0xC0
#define MQTT_PINGRESP		0xD0
#define MQTT_DISCONECT		0xE0

#define MQTT_HEADER_SIZE	2

extern void parse_mqtt(int sock);

typedef enum {
	CONN_START
} r_mqtt_state;

typedef struct {
	int sock;
	r_mqtt_state state;
} r_mqtt_conn;

#endif
