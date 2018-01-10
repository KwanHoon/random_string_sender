#ifndef FORMATTER_H_
#define FORMATTER_H_

#include <stdarg.h>

#define JSON_MSG_SIZE 8096

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
int make_json_msg(char *buf, enum jsontype type, const char *arr_name, size_t count, ...);

void release_json_msg(char *json_msg);

#endif
