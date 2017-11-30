#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#include "converter.h"

int main(int argc, char *argv[])
{
	struct cfg_info cfg;
	struct str_with_ts rand_str;
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

	micro_interval = cfg.interval * 1000;
	
	if(init_str_with_ts(&rand_str, &cfg) != 0) {
		fprintf(stderr, "Failed to init string\n");
		return -1;
	}

	if(init_converter(&converter, &cfg) != 0) {
		fprintf(stderr, "Failed to init converter\n");
		return -1;
	}

	//pthread_mutex_lock(&converter.sync_mutex);
	cvt_thr_id = pthread_create(&convert_thread, NULL, convert_json, (void *)&converter);
		
	// wait for http connection
	//pthread_cond_wait(&converter.sync_cond, &converter.sync_mutex);

	// start to make random string
	fprintf(stderr, "Wait connection\n");
	while(!converter.is_connected);
	fprintf(stderr, "Generate a random string.\n");	
	while(!gen_rand_str(&rand_str)) {
		elem.data = &rand_str;
		usleep(micro_interval);
		//fprintf(stderr, "   rand_str: %s\n", rand_str.rand_str);
		//pthread_mutex_lock(&converter.queue->mtx);
		if(enqueue(converter.queue, elem) != 0) {
			//fprintf(stderr, "Failed to enqueue\n");
		}	
		//pthread_mutex_unlock(&converter.queue->mtx);
	}
	fprintf(stderr, "End to make random string\n");

	//pthread_mutex_unlock(&converter.sync_mutex);	
	pthread_join(convert_thread, (void **)&status);

	return 0;
}
