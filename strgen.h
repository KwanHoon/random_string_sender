#ifndef STR_GEN_H__
#define STR_GEN_H_

#include <time.h>

#include "config.h"

#define TIMESTAMP_LEN 100

struct str_with_tm_t
{
	char *str;
	size_t len;

	char *tm_str;
	size_t tm_len;
	char tm_fmt[20];

	char *fullstr;
};

int init_str_with_tm(struct str_with_tm_t *str_with_tm, struct cfg_info *cfg);
int release_str_with_tm(struct str_with_tm_t *str_with_tm);

int make_rand_str(struct str_with_tm_t *str_with_tm);

#endif
