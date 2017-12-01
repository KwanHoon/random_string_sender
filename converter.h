#ifndef CONVERTER_H_
#define CONVERTER_H_

#include "queue.h"
#include "strgen.h"

struct convert_t
{
	struct Queue *queue;
		
	char *str;
	size_t len;

	char host[1024];
	short port;
	int is_connected;

	size_t int_time;
	size_t int_count;

	pthread_mutex_t conn_mutex;
	pthread_cond_t conn_cond;

	pthread_mutex_t sync_mutex;
	pthread_cond_t sync_cond;
};

// thread function
int init_converter(struct convert_t *converter, struct cfg_info *cfg);
int release_converter(struct convert_t *converter);
void *convert_json(void *arg);

char *make_kv(char *key, size_t klen, char *value, size_t vlen);

#endif
