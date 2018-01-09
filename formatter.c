#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "formatter.h"

// NOTE: a pair must be allocated by caller
int make_kv(char *pair, const char *key, const char *value)
{
	char *kv_fmt = "\"%s\":\"%s\"";

	if(key == NULL || value == NULL) {
		fprintf(stderr, "key or value is null\n");
		return -1;	
	}
	if(pair == NULL) {
		fprintf(stderr, "key-value pair buffer is null\n");
		return -1;
	}

	sprintf(pair, kv_fmt, key, value);

	//fprintf(stderr, "[debug] key:value {%s}\n", kv_buf);

	return 0;
}

char *make_json_msg(enum jsontype type, char *arr_name, size_t count,   ...)
{
	int i = 0;
	char *result_str = NULL;
	char *str = NULL;
	char tmp[5120];
	char *obj_fmt = "{%s}";
	char *arr_fmt = "\"%s\":[%s]";
	size_t len = 0;
	size_t tmp_len = 0;
	size_t total_len = 0;
	va_list args;

	// to get length of all strings
	va_start(args, count);
	for(i = 0; i < count; i++) {
		str = va_arg(args, char *);
		len = strlen(str);
		total_len += len;	
	}
	va_end(args);

	memset(tmp, '\0', total_len + count);

	va_start(args, count);
	for(i = 0; i < count; i++) {
		str = va_arg(args, char *);
		strncat(tmp, str, strlen(str));
		tmp_len += strlen(str);
		if(tmp_len > total_len + count) {
			fprintf(stderr, "Invalid arguments count or string length\n");
			return NULL;
		}
		tmp[tmp_len++] = ',';
	}
	va_end(args);
	tmp[tmp_len - 1] = '\0';

	result_str = (char *)malloc(total_len + count + 2);
	if(result_str == NULL) {
		fprintf(stderr, "Failed to alloc string memory2\n");
		return NULL;
	}
	memset(result_str, '\0', total_len + count + 2);

	if(type == json_obj)
		sprintf(result_str, obj_fmt, tmp);
	else if(type == json_arr)
		sprintf(result_str, arr_fmt, arr_name, tmp);	// TODO: arr_name null check.
	else {
		fprintf(stderr, "Invalid json type\n");
		return NULL;
	}

	//fprintf(stderr, "[debug] json result: %s\n", result_str);

	return result_str;

}

void release_json_msg(char *json_msg)
{
	if(json_msg) {
		free(json_msg);
		json_msg = NULL;
	}
}
