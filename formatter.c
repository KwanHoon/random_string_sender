#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "formatter.h"

char *make_kv(const char *key, size_t klen, const char *value, size_t vlen)
{
	char *kv_buf = NULL;
	char *kv_fmt = "\"%s\":\"%s\"";

	if(key == NULL || value == NULL) {
		fprintf(stderr, "key or value is null\n");
		return NULL;	
	}

	kv_buf = (char *)malloc(klen + vlen + 5);
	if(kv_buf == NULL) {
		perror("Failed alloc key value buffer");
		return NULL;

	}
	sprintf(kv_buf, kv_fmt, key, value);

	//fprintf(stderr, "[debug] key:value {%s}\n", kv_buf);

	return kv_buf;
}

char *make_array(size_t count, ...)
{
	int i = 0;
	char *result_str = NULL;
	char *str = NULL;
	char *tmp = NULL;
	char *arr_fmt = "[%s]";
	size_t len = 0;
	size_t total_len = 0;
	va_list args;

	va_start(args, count);
	for(i = 0; i < count; i++) {
		str = va_arg(args, char *);
		len = strlen(str);
		total_len += len;
	}
	va_end(args);

	tmp = (char *)malloc(total_len + count);
	if(tmp == NULL) {
		fprintf(stderr, "Failed to alloc string memory\n");
		return NULL;
	}

	va_start(args, count);
	for(i = 0; i < count; i++) {
		str = va_arg(args, char *);
		strcat(tmp, str);
		tmp[strlen(str)] = ',';
	}
	tmp[total_len + count] = '\0';

	result_str = (char *)malloc(total_len + count + 2);
	if(result_str == NULL) {
		fprintf(stderr, "Failed to alloc string memory2\n");
		return NULL;
	}
	sprintf(result_str, arr_fmt, tmp);
	free(tmp);

	return result_str;
}

char *make_obj(size_t count, ...)
{
	int i = 0;
	char *result_str = NULL;
	char *str = NULL;
	char *tmp = NULL;
	char *obj_fmt = "{%s}";
	size_t len = 0;
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

	tmp = (char *)malloc(total_len + count);	
	if(tmp == NULL) {
		fprintf(stderr, "Failed to alloc string memory\n");
		return NULL;
	}

	va_start(args, count);
	for(i = 0; i < count; i++) {
		str = va_arg(args, char *);
		strcat(tmp, str);
		tmp[strlen(str)] = ',';
		//fprintf(stderr, "[debug] str: %s\n", tmp);
	}
	tmp[total_len + count] = '\0';

	result_str = (char *)malloc(total_len + count + 2);
	if(result_str == NULL) {
		fprintf(stderr, "Failed to alloc string memory2\n");
		return NULL;
	}
	sprintf(result_str, obj_fmt, tmp);
	free(tmp);

	fprintf(stderr, "[debug] result: %s\n", result_str);

	return result_str;
}
