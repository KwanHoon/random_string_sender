#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	fprintf(stderr, "config, count: %lu, time: %lu\n", cfg->send_int_count, cfg->send_int_time);

	if(converter->int_time < 0) {
		fprintf(stderr, "Invalid interval time. Should be set larger than 0");
		return -1;
	}

	if(converter->int_count < 0) {
		fprintf(stderr, "Invalid interval count. Should be set larger than 0");
		return -1;
	}

	if(converter->int_count > 0 && converter->int_time > 0) {
		fprintf(stderr, "You only have to choose between the two of interval count or interval time\n");
		fprintf(stderr, "interval count: %lu, interval time: %lu\n", converter->int_count, converter->int_time);
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

int init_send_sync(struct convert_t *converter, struct sender_t *sender)
{
	if(converter == NULL || sender == NULL) {
		fprintf(stderr, "Failed to init send sync mutex, null pointer.\n");
		return -1;
	}

	converter->sender = sender;

	return 0;
}

void *convert_func(void *arg)
{
	struct convert_t *converter = (struct convert_t *)arg;
	char str_k[] = "string";
	char tm_k[] ="timestamp";
	char stm_k[] = "start-time";
	char *stm_v = NULL;
	char etm_k[] = "end-time";
	char *etm_v = NULL;
	char count_k[] = "count";
	size_t count_v = 0;
	char count_str[100];
	char items_k[] = "items";
	char **items = NULL;
	char *item_obj = NULL;
	char *tmp_obj = NULL;
	size_t obj_len = 0;
	char *total_obj = NULL;

	char *start_tm_kv = NULL;
	char *end_tm_kv = NULL;
	char *count_kv = NULL;
	char *items_arr = NULL;
	size_t n = 0;

	char *str_kv = NULL;
	char *tm_kv = NULL;
	char *str_tm_obj = NULL;
	char *stm_kv = NULL;		// start time key-value
	char *etm_kv = NULL;		// end time key-value 

	struct str_with_tm_t rand_str;
	struct str_with_tm_t send_str;

	if(converter == NULL) {
		fprintf(stderr, "converter is null\n");
		return NULL;
	}

	while(1) {
		pthread_mutex_lock(&converter->sync_mutex);
		pthread_cond_wait(&converter->sync_cond, &converter->sync_mutex);
		while(converter->queue->size) {
			rand_str = dequeue(converter->queue);
			pthread_mutex_unlock(&converter->sync_mutex);
			
			//if(str_kv == NULL)
				str_kv = (char *)malloc(strlen(str_k) + strlen(rand_str.str));
			//if(tm_kv == NULL)
				tm_kv = (char *)malloc(strlen(tm_k) + strlen(rand_str.tm_str));
			if(str_kv == NULL || tm_kv == NULL) {
				fprintf(stderr, "Faild to alloc memory\n");
				break;
			}
			memset(str_kv, '\0', strlen(str_k) + strlen(rand_str.str));
			memset(tm_kv, '\0', strlen(tm_k) + strlen(rand_str.tm_str));

			make_kv(str_kv, str_k, rand_str.str);
			make_kv(tm_kv, tm_k, rand_str.tm_str);
			//fprintf(stderr, "\n%s\n%s\n", str_kv, tm_kv); // default. only one element
			if(converter->int_count == 0 && converter->int_time == 0) {
				str_tm_obj = make_json_msg(json_obj, NULL, 1, str_kv, tm_kv);	
			}
			else {
				// set start & end time
				if(stm_v == NULL) {
					stm_v = rand_str.tm_str;
					//fprintf(stderr, "stm: %s\n", stm_v);
				}
				else if(etm_v == NULL && converter->queue->size == 0){
					etm_v = rand_str.tm_str;	
					//fprintf(stderr, "etm: %s\n", etm_v);
				}

				//if(converter->int_count > 0) {
					if(count_v == 0) {
						count_v = converter->queue->size + 1;
						sprintf(count_str, "%lu", count_v);
												
						items = (char **)malloc(sizeof(char *) * count_v);
						if(items == NULL) {
							perror("Failed to alloc memory(items)");
							break;
						}	
					}

					// make random string with timestamp
					if(items != NULL) {
						*items = make_json_msg(json_obj, NULL, 2, str_kv, tm_kv);
						//fprintf(stderr, "[debug] item1: %s\n", *items);	
						items++;
						release_json_msg(str_kv);
						release_json_msg(tm_kv);
					}
				//}	
			}
		}
		items -= count_v;

		count_kv = (char *)malloc(strlen(count_k) + strlen(count_str));
		if(count_kv == NULL) {
			fprintf(stderr, "Failed to alloc memory\n");
			break;
		}
		make_kv(count_kv, count_k, count_str);

		stm_kv = (char *)malloc(strlen(stm_k) + strlen(stm_v));
		etm_kv = (char *)malloc(strlen(etm_k) + strlen(etm_v));
		make_kv(stm_kv, stm_k, stm_v);
		make_kv(etm_kv, etm_k, etm_v);

		// make item array
		item_obj = (char *)malloc(strlen(items[0]) * count_v + count_v);
		memset(item_obj, '\0', strlen(items[0]) * count_v + count_v);
		for(n = 0; n < count_v; n++) {	
			strncat(item_obj, items[n], strlen(items[n]));
			strncat(item_obj, ",", 1);
		}	
		item_obj[strlen(item_obj) - 1] = '\0';
		items_arr = make_json_msg(json_arr, items_k, 1, item_obj);
		release_json_msg(item_obj);
		//fprintf(stderr, "[debug] item array: \n%s\n", items_arr);

		total_obj = make_json_msg(json_obj, NULL, 4, stm_kv, etm_kv, count_kv, items_arr);	
		release_json_msg(stm_kv);
		stm_v = NULL;
		release_json_msg(etm_kv);
		etm_v = NULL;
		release_json_msg(count_kv);
		release_json_msg(items_arr);
		items_arr = NULL;
		//fprintf(stderr, "[debug] total obj: \n%s\n", total_obj);

		memset(&send_str, 0x00, sizeof(send_str));
		send_str.fullstr = total_obj;
		send_str.fulllen = strlen(total_obj);
		release_json_msg(total_obj);

		// notify to sender
		pthread_mutex_lock(&converter->sender->send_sync_mtx);
		if(enqueue(converter->sender->queue, &send_str) < 0) {
			fprintf(stderr, "Failed to insert sender queue\n");
			pthread_mutex_unlock(&converter->sender->send_sync_mtx);
			break;
		}
		pthread_cond_signal(&converter->sender->send_sync_cond);
		pthread_mutex_unlock(&converter->sender->send_sync_mtx);
		
		count_v = 0;
		free(items);
		items = NULL;
	}

	fprintf(stderr, "[debug] convert thread is end\n");
}
