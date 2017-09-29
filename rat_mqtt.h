#ifndef RAT_MQTT_H
#define RAT_MQTT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "rat.h"
#include "rat_log.h"
#include "rat_string.h"

#define MQTT_CONNECT		0x10
#define MQTT_CONNACK		0x20

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
