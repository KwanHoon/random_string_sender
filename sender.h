#ifndef SENDER_H_
#define SENDER_H_

#include <pthread.h>
#include <regex.h>

#include "queue.h"
#include "config.h"

#define MAX_RECVBUF_SIZE 10240
#define IP_MATCH "([0-9])\.([0-9])\.([0-9])\.([0-9])"

struct sender_t
{
	struct Queue *queue;
	
	regex_t ip_regex;

	char host[1024];
	char hosturl[1024];
	char hostparam[1024];
	short port;

	int is_connected;

	pthread_mutex_t send_sync_mtx;
	pthread_cond_t send_sync_cond;
};

int init_regex(regex_t *regex);
int match_ip(regex_t *regex, const char *ip);
int init_sender(struct sender_t *sender, struct cfg_info *cfg);

// thread function
void *send_func(void *arg);

#endif
