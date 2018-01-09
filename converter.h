#ifndef CONVERTER_H_
#define CONVERTER_H_

//#include "queue.h"
#include "sender.h"
#include "queue.h"

struct convert_t
{
	struct sender_t *sender;
	struct Queue *queue;
		
	char *str;
	size_t len;

	size_t int_time;
	size_t int_count;

	pthread_mutex_t sync_mutex;
	pthread_cond_t sync_cond;

	//pthread_mutex_t *send_sync_mtx;
	//pthread_cond_t *send_sync_cond;
};

int init_converter(struct convert_t *converter, struct cfg_info *cfg);
int release_converter(struct convert_t *converter);

int int_send_sync(struct convert_t *converter, struct sender_t *sender);

void *convert_func(void *arg);



#endif
