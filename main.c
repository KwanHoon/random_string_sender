#include <stdio.h>

#include "converter.h"

int main(int argc, char *argv[])
{
	struct cfg_info cfg;
	struct str_with_tm_t rand_str;
	struct convert_t converter;
	struct sender_t sender;

	pthread_t convert_thread;
	pthread_t send_thread;

	int cvt_thr_id = 0;
	int snd_thr_id = 0;
	int status = 0;

	size_t micro_interval = 0;
	struct timeval start_t;
	struct timeval end_t;
	double diff_t = 0.0;

	fprintf(stderr, "Start main\n");

	fprintf(stderr, "Load config\n");
	if(load_config(NULL, &cfg) != 0) {
		fprintf(stderr, "Failed to load config\n");
		return -1;
	}
	if(cfg.str_int != 0)
		micro_interval = cfg.str_int * 1000;
	
	if(init_str_with_tm(&rand_str, &cfg) != 0) {
		fprintf(stderr, "Failed to init string\n");
		return -1;
	}

	if(init_sender(&sender, &cfg) != 0) {
		fprintf(stderr, "Failed to init sender\n");
		return -1;
	}

	if(init_converter(&converter, &cfg) != 0) {
		fprintf(stderr, "Failed to init converter\n");
		return -1;
	}

	if(init_send_sync(&converter, &sender) != 0) {
		fprintf(stderr, "Failed to init sync\n");
		return -1;
	}

	snd_thr_id = pthread_create(&send_thread, NULL, send_func, (void *)&sender);
	cvt_thr_id = pthread_create(&convert_thread, NULL, convert_func, (void *)&converter);
	
	// TODO. temp, should be edited!
	fprintf(stderr, "Wait connection\n");
	while(!sender.is_connected);

	// start to make random string
	fprintf(stderr, "Start to generate a random string.\n");	
	while(!make_rand_str(&rand_str)) {
		if(!diff_t)
			gettimeofday(&start_t, NULL);

		if(micro_interval != 0)
			usleep(micro_interval);

		pthread_mutex_lock(&converter.sync_mutex);

		if(enqueue(converter.queue, &rand_str) == 0) {
			if(converter.int_count == 0) {	// Converter get only one element.
				pthread_cond_signal(&converter.sync_cond);
			}
			else {
				if(converter.queue->size == converter.int_count)
					pthread_cond_signal(&converter.sync_cond);
				else {
					pthread_mutex_unlock(&converter.sync_mutex);
					continue;
				}
			}

			// check a interval time
			gettimeofday(&end_t, NULL);
			diff_t = ((double)end_t.tv_sec * 1000 + (double)end_t.tv_usec / 1000)
				- ((double)start_t.tv_sec * 1000 + (double)end_t.tv_usec / 1000);
			if(diff_t < converter.int_time) {
				pthread_mutex_unlock(&converter.sync_mutex);
				continue;	
			}
			else {
				//fprintf(stderr, "signal 2\n");
				diff_t = 0.0;
				pthread_cond_signal(&converter.sync_cond);
			}
		}
		else	
			fprintf(stderr, "Failed to enqueue\n");

		release_str_with_tm(&rand_str);
		pthread_mutex_unlock(&converter.sync_mutex);	
	}
	fprintf(stderr, "End to make random string\n");

	release_converter(&converter);

	pthread_join(convert_thread, (void **)&status);
	pthread_join(send_thread, (void **)&status);

	return 0;
}
