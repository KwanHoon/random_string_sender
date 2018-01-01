#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#include "converter.h"
//#include "sender.h"


struct sender_t
{
	struct queue *queue;
};

int main(int argc, char *argv[])
{
	struct cfg_info *cfg = NULL;
	struct str_with_ts rand_str;
	struct convert_t converter;
	struct sender_t sender;

	pthread_t convert_thread;
	int cvt_thr_id = 0;

	fprintf(stderr, "Start main\n");

	cvt_thr_id = pthread_create(&convert_thread, NULL, convert_json, (void *)&converter);

	rand_str.str = NULL;
	rand_str.len = 0;
	rand_str.tm_str = NULL;
	rand_str.tm_len = NULL;
	rand_str.rand_str = NULL;
	
	converter.queue = create_new_queue(0);
	sender.queue = create_new_queue(0);	

	fprintf(stderr, "Generate a random string.\n");
	while(!gen_rand_str(&rand_str)) {
		getchar(); // temp

		if(enqueue(converter.queue, (void *)&rand_str) != 0) {

		}
			
	}
	
	getchar();
	return 0;
}
