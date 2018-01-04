#include <stdio.h>
#include <string.h>

#include "formatter.h"

int main()
{
	char *test_str = NULL;
	const char *test_k1 = "key1";
	const char *test_v1 = "value1";
	const char *test_k2 = "key2";
	const char *test_v2 = "value2";

	printf("start test\n");

	test_str = make_obj(2, 
			make_kv(test_k1, strlen(test_k1), test_v1, strlen(test_v1)),
			make_kv(test_k2, strlen(test_k2), test_v2, strlen(test_v2))
			);

	fprintf(stderr, "result: %s\n", test_str);

	return 0;
}
