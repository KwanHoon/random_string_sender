#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "str_gen.h"

int init_str_with_ts(struct str_with_ts *str_with_ts, struct cfg_info *cfg)
{
	size_t total_len = 0;

	if(str_with_ts == NULL || cfg == NULL) {
		fprintf(stderr, "Init value is null\n");
		return -1;
	}

	str_with_ts->len = cfg->len;
	str_with_ts->tm_len = TIMESTAMP_LEN;
	memcpy(str_with_ts->tm_fmt, cfg->timestamp_fmt, sizeof(str_with_ts->tm_fmt));

	total_len = str_with_ts->len + 1 + str_with_ts->tm_len + 1;

	fprintf(stderr, "  total len: %d\n", total_len);

	str_with_ts->rand_str = (char *)malloc(total_len);
	str_with_ts->str =(char *)malloc(str_with_ts->len);
	str_with_ts->tm_str = (char *)malloc(str_with_ts->tm_len);

	if(str_with_ts->rand_str == NULL
	  || str_with_ts->str == NULL
	  || str_with_ts->tm_str == NULL) {
		fprintf(stderr, "Failed to alloc string memory\n");
		return -1;
	}

	memcpy(str_with_ts->tm_fmt, cfg->timestamp_fmt, sizeof(str_with_ts->tm_fmt));

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
	char rand_str_fmt[] = "%s,%s";
	time_t t;
	size_t total_len = 0;
	size_t n = 0;

	char temp[100];

	
	if(str_with_ts == NULL) {
		fprintf(stderr, "string is null\n");
		return -1;
	}

	total_len = str_with_ts->len + 1 + str_with_ts->tm_len + 1;

	memset(str_with_ts->rand_str, '\0', total_len);
	memset(str_with_ts->str, '\0', str_with_ts->len);
	memset(str_with_ts->tm_str, '\0', str_with_ts->tm_len);

	// get local current time
	t = time(NULL);
	timestamp = localtime(&t);
	if(strftime(str_with_ts->tm_str, str_with_ts->tm_len, str_with_ts->tm_fmt, timestamp) == 0) {
		perror("Failed to get timestamp");
		return -1;
	}
	//fprintf(stderr, "[debug] timestamp: {%s}\n", str_with_ts->tm_str);
	
	// make random string
	if(str_with_ts->str) {
		for(n = 0; n < str_with_ts->len - 1; n++) {
			int key = rand() % (int)(sizeof(charset) - 1);
			str_with_ts->str[n] = charset[key];
		}
	}
	//fprintf(stderr, "[debug] random string: {%s}\n", str_with_ts->str);
	
	// copy random string & timestamp
	sprintf(str_with_ts->rand_str, rand_str_fmt, str_with_ts->str, str_with_ts->tm_str);
	fprintf(stderr, "[debug] total string: {%s}, len: {%lu}\n", str_with_ts->rand_str, total_len);	
	//
	return 0;	
}
