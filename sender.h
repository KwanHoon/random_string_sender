#ifndef SENDER_H_
#define SENDER_H_

#include <pthread.h>

#include "queue.h"
#include "config.h"

struct sender_t
{
	struct Queue *queue;

	char host[1024];
	short port;
	int is_connected;

	pthread_mutex_t send_sync_mtx;
	pthread_cond_t send_sync_cond;
};

int init_sender(struct sender_t *sender, struct cfg_info *cfg);

// thread function
void *send_func(void *arg);

//int (*write_cb)(void *);

#endif
