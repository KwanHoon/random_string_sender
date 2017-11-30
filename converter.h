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

	char host[1024];
	short port;
	int is_connected;

	//pthread_mutex_t sync_mutex;
	//pthread_cond_t sync_cond;
};

// thread function
int init_converter(struct convert_t *converter, struct cfg_info *cfg);
void *convert_json(void *arg);

char *make_json_obj(char *kv);
char *make_kv(char *key, size_t klen, char *value, size_t vlen);

#endif
