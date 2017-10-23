#include "rat_backend.h"

int
store_data(subscription *sub)
{
	redisContext *conn = NULL;
	redisReply *resp = NULL;
	topic_entry t;
	uint32_t i;
	char value[9];
	uint8_t *key;

	conn = redisConnect("127.0.0.1", 6379);
	if ((conn != NULL) && conn->err) {
		printf("error : %s\n", conn->errstr);
		redisFree(conn);
		exit(-1);
	} else if (conn == NULL) {
		exit(-1);
	}

	key = (uint8_t*)malloc(sizeof(uint8_t) * sub->topic.l);
	strncpy((char*)key, (char*)sub->topic.d, sub->topic.l);
	SCAN_SUBSCRIPTION(value, sub);

	resp = (redisReply*)redisCommand(conn, "sadd %s %s", key, value);
	if (resp == NULL) {
		redisFree(conn);
		free(key);
		exit(-1);
	} else if (resp->type == REDIS_REPLY_ERROR) {
		freeReplyObject(resp);
		redisFree(conn);
		free(key);
		exit(-1);
	}

	resp = redisCommand(conn, "smembers %s", key);
	if (resp->type == REDIS_REPLY_ERROR) {
		printf("Error: %s\n", resp->str);
	} else if (resp->type != REDIS_REPLY_ARRAY) {
		printf("Unexpected type: %d\n", resp->type);
	} else {
		for (i = 0; i < resp->elements; i++) {
			PARSE_TOPIC(t, resp->element[i]->str);
			DUMP_TOPIC(t);
		}
	}

	freeReplyObject(resp);
	redisFree(conn);
	free(key);
	return 0;
}
