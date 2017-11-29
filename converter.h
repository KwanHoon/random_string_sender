#ifndef CONVERTER_H_
#define CONVERTER_H_

#include "queue.h"
#include "str_gen.h"

struct convert_t
{
	//struct queue *queue;
	struct Queue *queue;
		
	char *str;
	size_t len;

	char http_server_ip[16];
	short http_server_port;
};

// thread function
int init_converter(struct convert_t *converter, struct cfg_info *cfg);
void *convert_json(void *arg);

char *make_json_obj(char *kv);
char *make_kv(char *key, size_t klen, char *value, size_t vlen);

#endif
