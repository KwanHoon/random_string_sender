#ifndef FORMATTER_H_
#define FORMATTER_H_
#include <stdarg.h>

enum jsontype
{
	json_arr,
	json_obj
};

/* 
 * "key":"value"
 */
int make_kv(char *pair, const char *key, const char*value);

/* 
 *  json array format
 * [ {"key1":"value1"},{"key2":"value2"}, ... ]
 */

/* 
 *  json object format
 * { "key1":"value1","key2":"value2", ... }
 */
char *make_json_msg(enum jsontype type, size_t count, ...);

void release_json_msg(char *json_msg);

#endif
