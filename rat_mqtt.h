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

extern void parse_mqtt(int sock);

typedef enum {
	CONNECTING
} r_mqtt_state;

typedef struct {
	int sock;
	r_mqtt_state state;
} r_mqtt_conn;

#endif
