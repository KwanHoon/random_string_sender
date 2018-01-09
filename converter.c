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

	converter->send_sync_mtx = &sender->send_sync_mtx;
	converter->send_sync_cond = &sender->send_sync_cond;

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
	char **tmp_items = NULL;
	char *item_obj = NULL;

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

	struct Element elem;
	struct str_with_tm_t *rand_str = NULL;

	if(converter == NULL) {
		fprintf(stderr, "converter is null\n");
		return NULL;
	}

	while(1) {
		pthread_mutex_lock(&converter->sync_mutex);
		pthread_cond_wait(&converter->sync_cond, &converter->sync_mutex);
		fprintf(stderr, "start dequeue\n");
		while((rand_str = (struct str_with_tm_t *)dequeue(converter->queue)) != NULL) {
			//pthread_mutex_unlock(&converter->sync_mutex);
			fprintf(stderr, "dequeue [size: %lu]\n", converter->queue->size);
			fprintf(stderr, "rand str: %s\n", rand_str->fullstr);
			
			if(str_kv == NULL)
				str_kv = (char *)malloc(strlen(str_k) + strlen(rand_str->str));
			if(tm_kv == NULL)
				tm_kv = (char *)malloc(strlen(tm_k) + strlen(rand_str->tm_str));
			if(str_kv == NULL || tm_kv == NULL) {
				fprintf(stderr, "Faild to alloc memory\n");
				break;
			}
			memset(str_kv, '\0', strlen(str_k) + strlen(rand_str->str));
			memset(tm_kv, '\0', strlen(tm_k) + strlen(rand_str->tm_str));

			make_kv(str_kv, str_k, rand_str->str);
			make_kv(tm_kv, tm_k, rand_str->tm_str);
			fprintf(stderr, "%s\n%s\n\n", str_kv, tm_kv);

			// default. only one element
			if(converter->int_count == 0 && converter->int_time == 0) {
				str_tm_obj = make_json_msg(json_obj, 1, NULL, str_kv, tm_kv);	
			}
			else {
				// set start & end time
				if(stm_v == NULL) {
					stm_v = rand_str->tm_str;
				}
				else {
						
				}

				// make count
				if(converter->int_count > 0) {
					if(count_v == 0) {
						count_v = converter->queue->size + 1;
						items = (char **)malloc(count_v);
						tmp_items = items;		// to save initial position

						sprintf(count_str, "%lu", count_v);
						count_kv = (char *)malloc(strlen(count_k) + strlen(count_str));
						if(count_kv == NULL) {
							fprintf(stderr, "Failed to alloc memory\n");
							break;
						}
						make_kv(count_kv, count_k, count_str);
						fprintf(stderr, "count: %s\n", count_kv);	
					}
					// make random string with timestamp
					if(items != NULL) {
						*items = make_json_msg(json_obj, 2, NULL, str_kv, tm_kv);
						fprintf(stderr, "[debug] item1: %s\n", *items);
						items++;
					}

				}
				else if(converter->int_time > 0) {
					
				}
			}
				
			/*
			if(str_kv)
				free(str_kv);
			if(tm_kv)
				free(tm_kv);
			
			if(str_tm_obj)
				free(str_tm_obj);
				*/
			
		}

		// make item array
		fprintf(stderr, "\n");
		item_obj = (char *)malloc(strlen(tmp_items[0]) * count_v + count_v);
		for(n = 0; n < count_v; n++) {
			strcpy(item_obj, ",");
			strcpy(item_obj, tmp_items[n]);
			fprintf(stderr, "[debug] item2: %s\n", item_obj);
		}
		item_obj[strlen(item_obj)] = '\0';
		fprintf(stderr, "item obj: %s\n", item_obj);
		items_arr = make_json_msg(json_arr, 1, items_k, item_obj);
		fprintf(stderr, "[debug] arr: \n%s\n", items_arr);

		pthread_mutex_unlock(&converter->sync_mutex);
	}

	fprintf(stderr, "[debug] convert thread is end\n");
}
