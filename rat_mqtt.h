#ifndef RAT_MQTT_H
#define RAT_MQTT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "rat_config.h"
#include "rat_net.h"
#include "rat_backend.h"
#include "rat.h"

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

extern void parse_mqtt(r_connection *c);
extern void handle_suback(r_connection *c);
extern void handle_subscribe(r_connection *c);
extern void handle_publish(r_connection *c);

typedef enum {
	CONNACK_ALLOW,
	CONNACK_DENY_PROTOCOL,
	CONNACK_DENY_ID,
	CONNACK_DENY_SERVER_ERROR,
	CONNACK_DENY_AUTH_FAILURE,
	CONNACK_DENY_NOT_PERMITTED
} MQTT_CONNACK_RC;

#endif
