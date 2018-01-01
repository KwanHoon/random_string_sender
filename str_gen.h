#ifndef STR_GEN_H__
#define STR_GEN_H_

#include <time.h>

#include "config.h"

#define STR_LEN 11
#define TIME_LEN 30
//#define TIME_FMT "%Y-%M-%D"
//#define CHARSET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

struct str_with_ts
{
	char *str;
	size_t len;

	char *tm_str;
	size_t tm_len;

	char *rand_str;
};

int alloc_str_with_ts(struct str_with_ts *str_with_ts, struct cfg_info *cfg);
int release_str_with_ts(struct str_with_ts *str_with_ts);

int gen_rand_str(struct str_with_ts *str_with_ts);

#endif
