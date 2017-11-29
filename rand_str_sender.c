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

	fprintf(stderr, "Start main\n");

	fprintf(stderr, "Load config\n");
	if(load_config(NULL, &cfg) != 0) {
		fprintf(stderr, "Failed to load config\n");
		return -1;
	}
	
	if(init_str_with_ts(&rand_str, &cfg) != 0) {
		fprintf(stderr, "Failed to init string\n");
		return -1;
	}

	if(init_converter(&converter, &cfg) != 0) {
		fprintf(stderr, "Failed to init converter\n");
		return -1;
	}

	cvt_thr_id = pthread_create(&convert_thread, NULL, convert_json, (void *)&converter);

	//converter.queue = create_new_queue(0);

	fprintf(stderr, "Generate a random string.\n");
	while(!gen_rand_str(&rand_str)) {
		elem.data = &rand_str;
		//sleep(1);
		//fprintf(stderr, "   rand_str: %s\n", rand_str.rand_str);
		if(enqueue(converter.queue, elem) != 0) {
			fprintf(stderr, "Failed to enqueue\n");
		}	
	}
	
	pthread_join(convert_thread, (void **)&status);

	getchar();
	return 0;
}
