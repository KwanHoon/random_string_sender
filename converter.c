#include "converter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* { 
 *    "string": "{random string}",
 *    "timestamp": "{timestamp}"
 * }
 */

char *make_kv(char *key, size_t klen, char *value, size_t vlen)
{
	char *kv = NULL;

	if(key == NULL || value == NULL) {
		fprintf(stderr, "key or value is null\n");
		return NULL;	
	}

	kv = (char *)malloc(klen + 2 + vlen + 2/* "" */ + 1/* : */);
	if(kv == NULL) {
		perror("Failed to alloc key:value");
		return NULL;
	}

	kv[0] = '\"';
	memcpy(&kv[1], key, klen);
	kv[klen + 1] = '\"';

	kv[klen + 2] = ':';
	kv[klen + 3] = '\"';
	memcpy(&kv[klen + 4], value, vlen);
	kv[klen + 4 + vlen + 1] = '\"';

	fprintf(stderr, "[debug] make key:value {%s}\n", kv);

	return kv;
}

char *make_json_obj(char *kv)
{
	char *json_obj = NULL;

	if(kv == NULL) {
		fprintf(stderr, "kv is null\n");
		return NULL;
	}
	
	json_obj = (char *)malloc(strlen(kv) + 3);
	json_obj[0] = '{';
	memcpy(json_obj, kv, strlen(kv));
	json_obj[strlen(kv)] = '}';
	json_obj[strlen(kv) + 1] = '\0';

	fprintf(stderr, "[debug] json obj: {%s}\n", json_obj);
	
	return json_obj;
}

void *convert_json(void *arg)
{
	struct convert_t *json_str = (struct convert_t *)arg;
	char json_str_key[] = "string";
	char json_tm_key[] ="timestamp";

	if(json_str == NULL) {
		fprintf(stderr, "json str is null\n");
		return NULL;
	}

	while(1) {
		if(is_empty(json_str->queue) != 0) {
			char *str_kv = NULL;
			char *tm_kv = NULL;
			struct queue_node *node = NULL;
			struct str_with_ts *rand_str = NULL;

			node = dequeue(json_str->queue);
			rand_str = (struct str_with_ts *)node->data;

			fprintf(stderr, "[debug] str: %s, tm: %s\n", rand_str->str, rand_str->tm_str);

			str_kv = make_kv(
					json_str_key, sizeof(json_str_key) - 1,
					rand_str->str, rand_str->len);
			
			tm_kv = make_kv(
					json_tm_key, sizeof(json_tm_key) - 1,
					rand_str->tm_str, rand_str->tm_len);

			
		}
	}
}
