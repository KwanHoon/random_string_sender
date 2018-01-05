#include <stdio.h>
#include <string.h>

#include "formatter.h"

int main()
{
	char *test_str1 = NULL;
	char *test_str2 = NULL;
	char *test_str3 = NULL;

	char *test_obj1 = NULL;
	char *test_obj2 = NULL;
	char *test_obj3 = NULL;

	char *test_arr1 = NULL;
	char *test_arr2 = NULL;

	const char *test_k1 = "key1";
	const char *test_v1 = "value1";
	const char *test_k2 = "key2";
	const char *test_v2 = "value2";

	char kv1[100];
	char kv2[100];

	printf("start test\n");

	
	// NOTE: some error is occured.
	//test_obj1 = make_json_msg(json_obj, 2, make_kv(test_k1, test_v1), make_kv(test_k2, test_v2));
	//fprintf(stderr, "obj1: %s\n", test_obj1);	

	make_kv(kv1, test_k1, test_v1);
	make_kv(kv2, test_k2, test_v2);

	test_obj2 = make_json_msg(json_obj, 1, kv1);
	fprintf(stderr, "obj2: %s\n", test_obj2);	

	test_obj3 = make_json_msg(json_obj, 2, kv1, kv2);
	fprintf(stderr, "obj3: %s\n", test_obj3);

	test_arr1 = make_json_msg(json_arr, 2, test_obj3, test_obj2);
	fprintf(stderr, "arr1: %s\n", test_arr1);


	return 0;
}
