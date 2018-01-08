#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

#include "converter.h"
#include "formatter.h"

int init_converter(struct convert_t *converter, struct cfg_info *cfg)
{
	if(converter == NULL || cfg == NULL) {
		fprintf(stderr, "init value for converter is null\n");
		return -1;
	}
	
	converter->int_time = cfg->send_int_time;
	converter->int_count = cfg->send_int_count;

	if(converter->int_time < 0) {
		fprintf(stderr, "Invalid interval time. Should be set larger than 0");
		return -1;
	}

	if(converter->int_count < 0) {
		fprintf(stderr, "Invalid interval count. Should be set larger than 0");
		return -1;
	}

	if(converter->int_size > 0 && converter->int_time > 0) {
		fpritnf(stderr, "You only have to choose between the two of interval count or interval time\n");
		return -1;
	}

	converter->queue = create_queue(MAX_QUEUE_SIZE);
	
	pthread_mutex_init(&converter->sync_mutex, NULL);
	pthread_cond_init(&converter->sync_cond, NULL);

	return 0;
}

int release_converter(struct convert_t *converter)
{
	if(converter == NULL) {
		fprintf(stderr, "converter is null\n");
		return -1;
	}

	release_queue(converter->queue);

	pthread_mutex_destroy(&converter->sync_mutex);
	pthread_cond_destroy(&converter->sync_cond);

	return 0;
}

int int_send_sync(struct convert_t *converter, struct sender_t *sender)
{
	if(converter == NULL || sender == NULL) {
		fprintf(stderr, "Failed to init send sync mutex, null pointer.\n");
		return -1;
	}

	converter->send_sync_mtx = &sender->send_sync_mtx;
	converter->send_sync_cond = &sender->send_sync_cond;

	return 0;
}

void *convert_json(void *arg)
{
	struct convert_t *converter = (struct convert_t *)arg;
	char json_str_k[] = "string";
	char json_tm_k[] ="timestamp";
	char start_time_k[] = "start-time";
	char end_time_k[] = "end-time";
	char count_k[] = "count";
	char items_k[] = "items";

	struct Element elem;
	struct str_with_tm_t *rand_str = NULL;

	if(converter == NULL) {
		fprintf(stderr, "converter is null\n");
		return NULL;
	}

	while(1) {
		pthread_mutex_lock(&converter->sync_mutex);
		pthread_cond_wait(&converter->sync_cond, &converter->sync_mutex);
		if((rand_str = (struct str_with_tm_t *)dequeue(converter->queue)) != NULL ) {
			pthread_mutex_unlock(&converter->sync_mutex);
			
			
		}
		else {
			pthread_mutex_unlock(&converter->sync_mutex);
		}
	}

	fprintf(stderr, "[debug] convert thread is end\n");
}
