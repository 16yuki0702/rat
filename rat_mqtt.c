#include "rat_mqtt.h"

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
