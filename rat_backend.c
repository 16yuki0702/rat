#include "rat_backend.h"

int
store_data(subscription *sub, rat_conf *conf)
{
	redisContext *conn = NULL;
	redisReply *resp = NULL;
	char value[9];
	uint8_t *key;

	conn = redisConnect(conf->redis_server->data, conf->redis_port);
	if ((conn != NULL) && conn->err) {
		printf("error : %s\n", conn->errstr);
		redisFree(conn);
		exit(-1);
	} else if (conn == NULL) {
		exit(-1);
	}

	key = (uint8_t*)malloc(sizeof(uint8_t) * sub->topic.l + 1);
	strncpy((char*)key, (char*)sub->topic.d, sub->topic.l);
	key[sub->topic.l] = '\0';
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

	freeReplyObject(resp);
	redisFree(conn);
	free(key);

	return 0;
}

int
publish_data(r_str *topic, uint8_t *data, uint32_t len, rat_conf *conf)
{
	redisContext *conn = NULL;
	redisReply *resp = NULL;
	uint8_t *key;
	uint8_t i;
	topic_entry t;

	conn = redisConnect(conf->redis_server->data, conf->redis_port);
	if ((conn != NULL) && conn->err) {
		printf("error : %s\n", conn->errstr);
		redisFree(conn);
		exit(-1);
	} else if (conn == NULL) {
		exit(-1);
	}

	key = (uint8_t*)malloc(sizeof(uint8_t) * topic->l + 1);
	strncpy((char*)key, (char*)topic->d, topic->l);
	key[topic->l] = '\0';

	resp = (redisReply*)redisCommand(conn, "smembers %s", key);
	if (resp->type == REDIS_REPLY_ERROR) {
		printf("Error: %s\n", resp->str);
	} else if (resp->type != REDIS_REPLY_ARRAY) {
		printf("Unexpected type: %d\n", resp->type);
	} else {
		for (i = 0; i < resp->elements; i++) {
			PARSE_TOPIC(t, resp->element[i]->str);
			write(t.sock, data, len);
		}
	}

	freeReplyObject(resp);
	redisFree(conn);
	free(key);

	return 0;
}
