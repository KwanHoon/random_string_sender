#ifndef FORMATTER_H_
#define FORMATTER_H_
#include <stdarg.h>

/* 
 * "key":"value"
 */
char *make_kv(const char *key, size_t klen, const char*value, size_t vlen);

/*
 * [ {"key1":"value1"},{"key2":"value2"}, ... ]
 */
char *make_array(size_t count, ...);

/*
 * { "key1":"value1","key2":"value2", ... }
 */
char *make_obj(size_t count, ...);

#endif
