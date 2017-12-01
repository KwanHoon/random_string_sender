#include <stdio.h>

#include "converter.h"

int main(int argc, char *argv[])
{
	struct cfg_info cfg;
	struct str_with_tm_t rand_str;
	struct convert_t converter;
	struct Element elem;

	pthread_t convert_thread;
	int cvt_thr_id = 0;
	int status = 0;

	size_t micro_interval = 0;

	fprintf(stderr, "Start main\n");

	fprintf(stderr, "Load config\n");
	if(load_config(NULL, &cfg) != 0) {
		fprintf(stderr, "Failed to load config\n");
		return -1;
	}

	micro_interval = cfg.str_int * 1000;
	
	if(init_str_with_tm(&rand_str, &cfg) != 0) {
		fprintf(stderr, "Failed to init string\n");
		return -1;
	}

	if(init_converter(&converter, &cfg) != 0) {
		fprintf(stderr, "Failed to init converter\n");
		return -1;
	}

	//pthread_mutex_lock(&converter.conn_mutex);
	cvt_thr_id = pthread_create(&convert_thread, NULL, convert_json, (void *)&converter);
		
	// wait for http connection
	//pthread_cond_wait(&converter.conn_cond, &converter.conn_mutex);
	//pthread_mutex_unlock(&converter.conn_mutex);

	// TODO. temp, should be edited!
	fprintf(stderr, "Wait connection\n");
	while(!converter.is_connected);

	// start to make random string
	fprintf(stderr, "Start to generate a random string.\n");	
	while(!make_rand_str(&rand_str)) {
		elem.data = &rand_str;
		usleep(micro_interval);

		pthread_mutex_lock(&converter.sync_mutex);
		if(enqueue(converter.queue, elem) == 0)
			pthread_cond_signal(&converter.sync_cond);
		else	
			fprintf(stderr, "Failed to enqueue\n");
		pthread_mutex_unlock(&converter.sync_mutex);
	}
	fprintf(stderr, "End to make random string\n");

	release_str_with_tm(&rand_str);
	release_converter(&converter);


	//pthread_mutex_unlock(&converter.sync_mutex);	
	pthread_join(convert_thread, (void **)&status);

	return 0;
}
