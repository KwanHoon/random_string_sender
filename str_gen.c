#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "str_gen.h"


int alloc_str_with_ts(struct str_with_ts *str_with_ts, struct cfg_info *cfg)
{
	/*
	if(cfg == NULL) {
		if(str == NULL) {
			fprintf(stderr, "string is null\n");
			return -1;
		}
		str_with_ts->str = (char *)malloc(MAX_STR_LEN);
		str_with_ts->str_len = MAX_STR_LEN;
		str_with_ts->timestamp = NULL;
	}
	else {
		fprintf(stderr "not implemented\n");
		return -1;
	}
	*/
	
	return 0;
}

int release_str_with_ts(struct str_with_ts *str_with_ts)
{
	if(str_with_ts == NULL) {
		fprintf(stderr, "Failed to release string. [string is null]\n");
		return -1;
	}

	if(str_with_ts->str == NULL){
		fprintf(stderr, "Failed to release random string\n");
		return -1;
	}

	free(str_with_ts->str);
	str_with_ts->str = NULL;

	return 0;
}

int gen_rand_str(struct str_with_ts *str_with_ts)
{
	const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	struct tm *timestamp;
	time_t t;
	size_t total_len = 0;

	str_with_ts->len = STR_LEN;
	str_with_ts->tm_len = TIME_LEN;

	total_len = str_with_ts->len + 1 + str_with_ts->tm_len + 1;

	if(str_with_ts == NULL) {
		fprintf(stderr, "string is null\n");
		return -1;
	}

	if(str_with_ts->rand_str == NULL)
		str_with_ts->rand_str = (char *)malloc(total_len);
	if(str_with_ts->str == NULL)
		str_with_ts->str =(char *)malloc(str_with_ts->len);
	if(str_with_ts->tm_str == NULL)
		str_with_ts->tm_str = (char *)malloc(str_with_ts->tm_len);

	memset(str_with_ts->rand_str, '\0', total_len);
	memset(str_with_ts->str, '\0', str_with_ts->len);
	memset(str_with_ts->tm_str, '\0', str_with_ts->tm_len);

	// get local current time
	t = time(NULL);
	timestamp = localtime(&t);
	strftime(str_with_ts->tm_str, str_with_ts->tm_len, "%c", timestamp);
	//fprintf(stderr, "[debug] timestamp: {%s}\n", str_with_ts->tm_str);
	
	if(str_with_ts->str) {
		for(size_t n = 0; n < str_with_ts->len; n++) {
			int key = rand() % (int)(sizeof(charset) - 1);
			str_with_ts->str[n] = charset[key];
		}
	}
	//fprintf(stderr, "[debug] random string: {%s}\n", str_with_ts->str);
	
	// copy random string & timestamp
	memcpy(str_with_ts->rand_str, str_with_ts->str, str_with_ts->len);
	str_with_ts->rand_str[str_with_ts->len + 1] = ',';	
	memcpy(str_with_ts->rand_str + str_with_ts->len + 2, str_with_ts->tm_str, str_with_ts->tm_len);
	str_with_ts->rand_str[total_len] = '\0';

	//fprintf(stderr, "[debug] total string: {%s}, len: {%lu}\n", str_with_ts->rand_str, total_len);

	/*
	fprintf(stderr, "[debug]");
	for(size_t i = 0; i < total_len; i++) {
		fprintf(stderr, "%c", str_with_ts->rand_str[i]);
	}
	fprintf(stderr, "\n");
	*/
		
	return 0;	
}
