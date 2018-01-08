#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "strgen.h"

int init_str_with_tm(struct str_with_tm_t *str_with_tm, struct cfg_info *cfg)
{
	size_t total_len = 0;

	if(str_with_tm == NULL || cfg == NULL) {
		fprintf(stderr, "Init value is null\n");
		return -1;
	}

	cfg->timestamp_fmt[strlen(cfg->timestamp_fmt)-1] = '\0';
	str_with_tm->len = cfg->len;
	str_with_tm->tm_len = TIMESTAMP_LEN;
	strcpy(str_with_tm->tm_fmt, cfg->timestamp_fmt);
	str_with_tm->tm_fmt[strlen(cfg->timestamp_fmt)] = '\0';

	total_len = str_with_tm->len + 1 + str_with_tm->tm_len + 1;

	str_with_tm->fullstr = (char *)malloc(total_len);
	str_with_tm->str =(char *)malloc(str_with_tm->len);
	str_with_tm->tm_str = (char *)malloc(str_with_tm->tm_len);

	if(str_with_tm->fullstr == NULL
	  || str_with_tm->str == NULL
	  || str_with_tm->tm_str == NULL) {
		fprintf(stderr, "Failed to alloc string memory\n");
		return -1;
	}

	return 0;
}

int release_str_with_tm(struct str_with_tm_t *str_with_tm)
{
	if(str_with_tm == NULL) {
		fprintf(stderr, "Failed to release string. [string is null]\n");
		return -1;
	}

	if(str_with_tm->str != NULL){
		free(str_with_tm->str);
		str_with_tm->str = NULL;
	}
	
	if(str_with_tm->tm_str != NULL) {
		free(str_with_tm->tm_str);
		str_with_tm->tm_str = NULL;
	}

	if(str_with_tm->fullstr != NULL) {
		free(str_with_tm->fullstr);
		str_with_tm->fullstr = NULL;
	}


	return 0;
}

int make_rand_str(struct str_with_tm_t *str_with_tm)
{
	const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	struct tm *timestamp;
	char fullstr_fmt[] = "%s,%s";
	time_t t;
	size_t total_len = 0;
	size_t n = 0;

	char temp[100];

	
	if(str_with_tm == NULL) {
		fprintf(stderr, "string is null\n");
		return -1;
	}

	total_len = str_with_tm->len + 1 + str_with_tm->tm_len + 1;

	memset(str_with_tm->fullstr, '\0', total_len);
	memset(str_with_tm->str, '\0', str_with_tm->len);
	memset(str_with_tm->tm_str, '\0', str_with_tm->tm_len);

	// get local current time
	timestamp = str_with_tm->timestamp;

	t = time(NULL);
	str_with_tm->time = t;
	timestamp = localtime(&t);
	if(strftime(str_with_tm->tm_str, str_with_tm->tm_len, str_with_tm->tm_fmt, timestamp) == 0) {
		perror("Failed to get timestamp");
		return -1;
	}
	//fprintf(stderr, "[debug] timestamp: {%s}\n", str_with_ts->tm_str);
	
	// make random string
	if(str_with_tm->str) {
		for(n = 0; n < str_with_tm->len - 1; n++) {
			int key = rand() % (int)(sizeof(charset) - 1);
			str_with_tm->str[n] = charset[key];
		}
	}
	//fprintf(stderr, "[debug] random string: {%s}\n", str_with_tm->str);
	
	// copy random string & timestamp
	sprintf(str_with_tm->fullstr, fullstr_fmt, str_with_tm->str, str_with_tm->tm_str);

	return 0;	
}
