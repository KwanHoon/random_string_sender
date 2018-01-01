#ifndef CONVERTER_H_
#define CONVERTER_H_

#include "queue.h"
#include "str_gen.h"

struct convert_t
{
	struct queue *queue;
		
	char *str;
	size_t len;
};

// thread function
void *convert_json(void *arg);

char *make_json_obj(char *kv);
char *make_kv(char *key, size_t klen, char *value, size_t vlen);

#endif
