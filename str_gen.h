#include <time.h>

struct str_with_ts
{
	char *str;
	unsigned str_len;
	struct tm *timestamp;	
}


int alloc_str_with_ts(struct str_with_ts *str);
int gen_rand_str(struct str_with_ts *str);
