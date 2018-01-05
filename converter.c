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

	if(converter->int_count < 1) {
		fprintf(stderr, "Invalid interval count. Should be set larger than 1");
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

			

			/*
			memset(total_json, '\0', sizeof(total_json));	
			memset(recv_buf, 0x00, sizeof(recv_buf));

			// send http message
			sprintf(http_header, http_header_fmt, ent->h_name, converter->port, strlen(total_json));
			if(write(sock, http_header, strlen(http_header), 0) < 0) {
				perror("Failed to send header");
			}
			else {
				if(write(sock, total_json, strlen(total_json)) < 0) {
					perror("Failed to send body");
				}
				fprintf(stderr, "[%lu]send: \n%s%s\n\n",
				  strlen(http_header) + strlen(total_json), http_header, total_json); 
			}

			if((recv_len = read(sock, recv_buf, sizeof(recv_buf))) < 0 ) {
				perror("Failed to recv");
			}

			fprintf(stderr, "[%lu]recv: \n%s\n\n",recv_len, recv_buf);
			*/
		}
		else {
			pthread_mutex_unlock(&converter->sync_mutex);
		}
	}

	fprintf(stderr, "[debug] convert thread is end\n");
}
